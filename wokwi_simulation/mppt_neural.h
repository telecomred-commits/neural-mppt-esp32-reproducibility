#pragma once
#include <cmath>
#include <cstdint>
#include "mppt_weights.h"
#ifndef MPPT_POWER_REFINEMENT
#define MPPT_POWER_REFINEMENT 1
#endif
#ifndef MPPT_ACTIVE_VERIFICATION
#define MPPT_ACTIVE_VERIFICATION 1
#endif
namespace mppt {
inline float clamp(float x,float lo,float hi){return x<lo?lo:(x>hi?hi:x);}
struct Decision {
 float load,delta,reference,error,predicted_current,model_error,parameter_change;
 uint32_t learning_updates;
 int source;
 bool reference_valid,at_limit,learned;
 float nn_reference,power_reference,power_slope,peak_noise_V,loss_before,loss_after,measured_slope;
 uint32_t power_checks,learning_rejections;
 int probe_phase,gradient_valid,learning_reason,verification_kind;
};
// Both modes run identical measured-power verification. learning_ only enables
// updates to neural parameters. No physical PV oracle is used in this file.
class Controller {
 public:
 explicit Controller(bool learning=true):learning_(learning){reset();}
 void reset(){
  for(int j=0;j<16;++j)out_[j]=mppt_weights::w1[j][0];
  bias_=mppt_weights::b1[0];updates_=rejections_=checks_=steps_=0;
  initialized_=false;stable_=0;count_=head_=0;phase_=phase_age_=near_=0;
  validated_=false;power_ref_=peak_noise_=fit_a_=fit_b_=fit_c_=fit_center_=fit_sigma_=0;
  check_step_=retry_step_=0;verification_kind_=0;
 }
 float predict(float v,float g,float t)const{
  float h[16];features(v,t,h);return clamp(normalized(h),0,1.5f)*14*g/1000;
 }
 float target(float g,float t)const{
  const float top=clamp(mppt_weights::voc[0]+mppt_weights::voc[1]*(t-37.5f)/12.5f,1,45);
  float best_v=0,best_p=-1;
  for(int n=0;n<=100;++n){const float v=top*n/100,p=v*predict(v,g,t);if(p>best_p){best_p=p;best_v=v;}}
  const float lo=clamp(best_v-top/100,0,top),hi=clamp(best_v+top/100,0,top);
  for(int n=0;n<=20;++n){const float v=lo+(hi-lo)*n/20,p=v*predict(v,g,t);if(p>best_p){best_p=p;best_v=v;}}
  return best_v;
 }
 Decision update(float v,float i,float g,float t,float load,float applied=-1){
  Decision d={};d.load=std::isfinite(load)?clamp(load,0,100):50;d.source=4;
  d.parameter_change=distance();d.learning_updates=updates_;d.learning_rejections=rejections_;d.power_checks=checks_;
  if(!std::isfinite(v)||!std::isfinite(i)||!std::isfinite(g)||!std::isfinite(t)||!std::isfinite(load)||v<0||i<0)return d;
  ++steps_;if(applied<0||!std::isfinite(applied))applied=load;
  const float disturbance=applied-load;
  const bool env_changed=initialized_&&(std::fabs(g-epoch_g_)>std::fmax(2.0f,.01f*g)||std::fabs(t-epoch_t_)>.15f);
  const bool load_changed=initialized_&&std::fabs(disturbance-prev_disturbance_)>.15f;
  if(!initialized_||env_changed){clear_verification();stable_=0;epoch_g_=g;epoch_t_=t;}
  else if(stable_<10000)++stable_;
  if(load_changed){stable_=0;phase_=phase_age_=near_=0;count_=head_=0;retry_step_=steps_+6;}
  const bool covered=g>=399&&g<=801&&t>=24.9f&&t<=50.1f;
  float step=0;
  if(covered){
   const float gc=clamp(g,400,800),tc=clamp(t,25,50);
   d.predicted_current=predict(v,gc,tc);d.model_error=i-d.predicted_current;
   fit_temp_=tc;
   float nn=target(gc,tc),control=nn;int command_phase=0;
#if MPPT_POWER_REFINEMENT
   if(validated_&&std::fabs(v-fit_center_)<1.6f){
    const float x=v-fit_center_,p=v*i*1000/g;
    if(std::fabs(p-(fit_c_+fit_b_*x+fit_a_*x*x))>std::fmax(2.0f,5*fit_sigma_)){
     clear_verification();stable_=0;retry_step_=steps_+6;
    }
   }
   if(stable_>=3&&!env_changed&&!load_changed){
    if(std::fabs(v-nn)<=1.5f){
     buffer_v_[head_]=v;buffer_p_[head_]=v*i*1000/g;head_=(head_+1)%32;if(count_<32)++count_;
    }
    if(!validated_&&phase_==0&&count_>=9&&try_fit(nn,false)){
     ++checks_;validated_=true;check_step_=steps_;verification_kind_=1;near_=0;
    }
    if(!validated_&&phase_==0){
     if(std::fabs(v-nn)<=.12f)++near_;else near_=0;
     if(MPPT_ACTIVE_VERIFICATION&&near_>=16&&steps_>=retry_step_){phase_=1;phase_age_=0;probe_center_=nn;near_=0;}
    }
    if(phase_){
     ++phase_age_;command_phase=phase_;control=clamp(probe_center_+(phase_==1?-.45f:phase_==2?.45f:0),1,44);
     if(std::fabs(v-control)<=.12f){
      if(++near_>=6){
       near_=0;++phase_;
       if(phase_==3){
        phase_=0;
        if(try_fit(probe_center_,true)){++checks_;validated_=true;check_step_=steps_;verification_kind_=2;}
        else retry_step_=steps_+80;
       }
      }
     }else near_=0;
     if(phase_age_>100){phase_=0;near_=0;retry_step_=steps_+80;}
    }
   }
   if(validated_&&phase_==0){
    control=verified_reference(nn);
    if(steps_-check_step_>240){validated_=false;count_=head_=0;near_=0;}
   }
#endif
   if(learning_){
    if(stable_>=3&&validated_&&phase_==0&&std::fabs(v-power_ref_)<1.2f&&steps_-check_step_<=120){
     learn(v,i,gc,tc,d,nn);if(d.learned)nn=d.nn_reference;
    }else d.learning_reason=2;
   }
   if(phase_==0)control=validated_?verified_reference(nn):nn;
   d.reference=control;d.reference_valid=true;d.error=v-control;
   d.source=phase_?5:validated_?2:1;
   step=std::fabs(d.error)<=.05f?0:clamp((phase_?1.5f:.5f)*d.error,-3,3);
   d.nn_reference=nn;d.power_reference=validated_?power_ref_:0;
   d.power_slope=validated_?model_slope(fit_center_,gc,tc):0;
   d.measured_slope=validated_?fit_b_*gc/1000:0;
   d.peak_noise_V=validated_?peak_noise_:0;d.power_checks=checks_;
   d.probe_phase=phase_?command_phase:0;d.gradient_valid=validated_?1:0;d.verification_kind=verification_kind_;
  }else{
   clear_verification();stable_=0;d.source=initialized_?3:0;d.learning_reason=8;
   if(initialized_){const float dv=v-prev_v_,di=i-prev_i_;
    if(std::fabs(dv)<.05f){if(std::fabs(di)>=.02f)step=di>0?-1.0f:1.0f;}
    else if(v>1e-6f){const float c=di/dv+i/v;if(std::fabs(c)>=.01f)step=c>0?-1.0f:1.0f;}
   }
  }
  const float before=d.load;d.load=clamp(before+step,0,100);d.delta=d.load-before;
  d.at_limit=d.load<=0||d.load>=100;d.learning_updates=updates_;d.parameter_change=distance();d.learning_rejections=rejections_;
  prev_v_=v;prev_i_=i;prev_g_=g;prev_t_=t;prev_disturbance_=disturbance;initialized_=true;return d;
 }
 private:
 void clear_verification(){validated_=false;phase_=phase_age_=near_=0;count_=head_=0;verification_kind_=0;retry_step_=steps_+6;}
 void features(float v,float t,float*h)const{
  const float x[2]={(v-21)/21,(t-37.5f)/12.5f};
  for(int j=0;j<16;++j){float s=mppt_weights::b0[j];for(int k=0;k<2;++k)s+=x[k]*mppt_weights::w0[k][j];h[j]=std::tanh(s);}
 }
 float normalized(const float*h)const{float y=bias_;for(int j=0;j<16;++j)y+=out_[j]*h[j];return y;}
 void slope_features(float v,float t,float*q)const{
  float h[16];features(v,t,h);
  for(int j=0;j<16;++j)q[j]=h[j]+v*(1-h[j]*h[j])*mppt_weights::w0[0][j]/21;
  q[16]=1;
 }
 float normalized_slope(const float*q)const{float y=bias_;for(int j=0;j<16;++j)y+=out_[j]*q[j];return y;}
 float model_slope(float v,float g,float t)const{float q[17];slope_features(v,t,q);return normalized_slope(q)*14*g/1000;}
 void project(float*d)const{
  for(int k=0;k<2;++k){
   float dot=0;for(int j=0;j<17;++j)dot+=mppt_weights::constraint_basis[k][j]*d[j];
   for(int j=0;j<17;++j)d[j]-=dot*mppt_weights::constraint_basis[k][j];
  }
 }
 float curvature(float v,float t)const{
  float h[16];features(v,t,h);float first=0,second=0;
  for(int j=0;j<16;++j){const float w=mppt_weights::w0[0][j]/21,dh=1-h[j]*h[j];first+=out_[j]*dh*w;second+=out_[j]*(-2*h[j]*dh*w*w);}
  return 14*(2*first+v*second);
 }
 bool try_fit(float center,bool active){
  // Fit measured P/(G/1000). Curvature prior stabilizes noisy short windows;
  // measured power supplies the local slope. No analytic PV MPP is used.
  double s[5]={},y[3]={};int n=0;float vmin=1e9f,vmax=-1e9f;
  for(int j=0;j<count_;++j){
   const double x=buffer_v_[j]-center,p=buffer_p_[j];if(std::fabs(x)>1.5)continue;
   double power=1;for(int k=0;k<5;++k){s[k]+=power;power*=x;}
   y[0]+=p;y[1]+=p*x;y[2]+=p*x*x;++n;vmin=std::fmin(vmin,buffer_v_[j]);vmax=std::fmax(vmax,buffer_v_[j]);
  }
  if(n<(active?9:16)||vmax-vmin<.6f)return false;
  const double aa=s[0],bb=s[1],cc=s[2],dd=s[3],ee=s[4]+2;
  const double prior=.5*curvature(center,fit_temp_);
  if(prior>=-.15||prior<-20)return false;
  y[2]+=2*prior;
  const double det=aa*(cc*ee-dd*dd)-bb*(bb*ee-cc*dd)+cc*(bb*dd-cc*cc);
  if(det<=1e-7)return false;
  const double inv[3][3]={{(cc*ee-dd*dd)/det,(cc*dd-bb*ee)/det,(bb*dd-cc*cc)/det},
   {(cc*dd-bb*ee)/det,(aa*ee-cc*cc)/det,(bb*cc-aa*dd)/det},
   {(bb*dd-cc*cc)/det,(bb*cc-aa*dd)/det,(aa*cc-bb*bb)/det}};
  if(inv[2][2]<=0)return false;
  double beta[3]={};for(int j=0;j<3;++j)for(int k=0;k<3;++k)beta[j]+=inv[j][k]*y[k];
  if(beta[2]>=-.15||beta[2]<-20)return false;
  double residual=0;
  for(int j=0;j<count_;++j){const double x=buffer_v_[j]-center;if(std::fabs(x)>1.5)continue;const double e=buffer_p_[j]-(beta[0]+beta[1]*x+beta[2]*x*x);residual+=e*e;}
  const double variance=std::fmax(.0025,residual/(n-3)),peak=center-beta[1]/(2*beta[2]);
  const double d1=-1/(2*beta[2]),d2=beta[1]/(2*beta[2]*beta[2]);
  const double prior_spread=.2*(peak-center);
  const double noise=std::sqrt(std::fmax(0.,variance*(d1*d1*inv[1][1]+2*d1*d2*inv[1][2]+d2*d2*inv[2][2]))+prior_spread*prior_spread);
  if(!std::isfinite(peak)||!std::isfinite(noise)||noise>(active?.10:.04))return false;
  if(peak<vmin-.08||peak>vmax+.08||std::fabs(peak-center)>1.0)return false;
  int left=0,right=0;
  for(int j=0;j<count_;++j){if(buffer_v_[j]<peak-.02)++left;if(buffer_v_[j]>peak+.02)++right;}
  if((left<2||right<2)&&!active&&(std::fabs(peak-center)>.08||noise>.03))return false;
  power_ref_=float(peak);peak_noise_=float(noise);fit_center_=center;
  fit_c_=float(beta[0]);fit_b_=float(beta[1]);fit_a_=float(beta[2]);fit_sigma_=float(std::sqrt(variance));return true;
 }
 float verified_reference(float nn)const{
  const float diff=power_ref_-nn;
  const float correction=std::fmax(0.0f,std::fabs(diff)-2*peak_noise_);
  return nn+(diff<0?-correction:correction);
 }
 void learn(float v,float i,float g,float t,Decision&d,float previous_reference=-1){
  float h[16],phi[17],q[17],di[17],dq[17];features(v,t,h);slope_features(fit_center_,t,q);
  for(int j=0;j<16;++j)phi[j]=h[j];
  phi[16]=1;
  for(int j=0;j<17;++j){di[j]=phi[j];dq[j]=q[j];}
  project(di);project(dq);
  const float scale=14*g/1000,e=i/scale-normalized(h),slope=normalized_slope(q)-fit_b_/14;
  const float ti=.04f/scale,tq=std::fmax(.01f,std::fabs(2*fit_a_)*peak_noise_/14);
  if(std::fabs(e)<=ti&&std::fabs(slope)<=tq){d.learning_reason=4;return;}
  float ni=1,nq=1;for(int j=0;j<17;++j){ni+=di[j]*di[j];nq+=dq[j]*dq[j];}
  float change[17];
  for(int j=0;j<17;++j)change[j]=.10f*(clamp(e,-.1f,.1f)*di[j]/ni-clamp(slope,-.1f,.1f)*dq[j]/nq);
  float alpha=1;
  for(int j=0;j<17;++j){
   const float initial=j<16?mppt_weights::w1[j][0]:mppt_weights::b1[0],current=j<16?out_[j]:bias_;
   if(change[j]>0)alpha=std::fmin(alpha,(initial+.25f-current)/change[j]);else if(change[j]<0)alpha=std::fmin(alpha,(initial-.25f-current)/change[j]);
  }
  alpha=clamp(alpha,0,1);float old[17];
  for(int j=0;j<16;++j)old[j]=out_[j];
  old[16]=bias_;
  const float old_ref=previous_reference>=0?previous_reference:target(g,t);
  const float loss=e*e/(ti*ti)+slope*slope/(tq*tq);d.loss_before=d.loss_after=loss;
  bool accepted=false;
  for(int attempt=0;attempt<5;++attempt){
   for(int j=0;j<16;++j)out_[j]=old[j]+alpha*change[j];
   bias_=old[16]+alpha*change[16];
   const float en=i/scale-normalized(h),qn=normalized_slope(q)-fit_b_/14,next_loss=en*en/(ti*ti)+qn*qn/(tq*tq);
   if(next_loss<loss-1e-5f&&std::fabs(en)<=std::fmax(std::fabs(e),ti)&&std::fabs(qn)<=std::fmax(std::fabs(slope),tq)){
    d.loss_after=next_loss;accepted=true;break;
   }
   alpha*=.5f;
  }
  if(accepted){
   const float new_ref=target(g,t);
   if(std::fabs(new_ref-power_ref_)>std::fabs(old_ref-power_ref_)+.015f)accepted=false;
   else d.nn_reference=new_ref;
  }
  if(accepted){
   float moved=0;for(int j=0;j<16;++j)moved+=(out_[j]-old[j])*(out_[j]-old[j]);
   moved+=(bias_-old[16])*(bias_-old[16]);
   if(moved>1e-16f){++updates_;d.learned=true;d.learning_reason=1;}else d.learning_reason=7;
  }else{
   for(int j=0;j<16;++j)out_[j]=old[j];
   bias_=old[16];++rejections_;d.learning_reason=6;d.loss_after=d.loss_before;
  }
 }
 float distance()const{
  float s=(bias_-mppt_weights::b1[0])*(bias_-mppt_weights::b1[0]);
  for(int j=0;j<16;++j){const float d=out_[j]-mppt_weights::w1[j][0];s+=d*d;}
  return std::sqrt(s);
 }
 float out_[16],bias_,prev_v_=0,prev_i_=0,prev_g_=0,prev_t_=0,prev_disturbance_=0;
 float epoch_g_=0,epoch_t_=0,fit_temp_=37;
 float buffer_v_[32]={},buffer_p_[32]={};
 float power_ref_=0,peak_noise_=0,fit_center_=0,fit_a_=0,fit_b_=0,fit_c_=0,fit_sigma_=0,probe_center_=0;
 uint32_t updates_=0,rejections_=0,checks_=0,steps_=0,check_step_=0,retry_step_=0;
 int stable_=0,count_=0,head_=0,phase_=0,phase_age_=0,near_=0,verification_kind_=0;
 bool learning_,initialized_=false,validated_=false;
};
}


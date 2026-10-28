#include <Arduino.h>
#include <cmath>
#include <cstdio>
/* ================= CONFIGURACION DEL EXPERIMENTO =================
   Cambie solo las lineas #define de este bloque.
   MPPT_MODE: 0=P&O, 1=INC, 2=P&O adaptativo, 3=neuronal V9.
   MPPT_ONLINE_LEARNING: 1=ONLINE, 0=FROZEN.
   Mantenga MPPT_ACTIVE_VERIFICATION=1 para comparar ONLINE/FROZEN.
*/
#define MPPT_PO 0
#define MPPT_INC 1
#define MPPT_ADAPT_PO 2
#define MPPT_LEARNING 3

#ifndef MPPT_MODE
#define MPPT_MODE MPPT_LEARNING
#endif
#ifndef MPPT_ONLINE_LEARNING
#define MPPT_ONLINE_LEARNING 0  // UNICO SELECTOR: 1=ONLINE, 0=FROZEN
#endif
#ifndef MPPT_ACTIVE_VERIFICATION
#define MPPT_ACTIVE_VERIFICATION 1
#endif
/* ================= FIN DE LOS SELECTORES ================= */

// Validaciones fijas. No cambie estas expresiones al seleccionar una prueba.
static_assert(MPPT_MODE >= MPPT_PO && MPPT_MODE <= MPPT_LEARNING,
              "MPPT_MODE must be 0, 1, 2, or 3");
static_assert(MPPT_ONLINE_LEARNING == 0 || MPPT_ONLINE_LEARNING == 1,
              "MPPT_ONLINE_LEARNING must be 0 or 1");
static_assert(MPPT_ACTIVE_VERIFICATION == 0 || MPPT_ACTIVE_VERIFICATION == 1,
              "MPPT_ACTIVE_VERIFICATION must be 0 or 1");

// El controlador y la cabecera serial usan exactamente la misma seleccion.
constexpr bool MPPT_LEARNING_ENABLED = (MPPT_ONLINE_LEARNING == 1);
#include "mppt_neural.h"

/* Original analog connections and verified Wokwi acquisition. */
const int PIN_V=34, PIN_I=35, PIN_G=33, PIN_T=32;
const float V_SENSE_MAX=50.0f, I_SENSE_MAX=15.0f;
const float G_MAX=1000.0f, T_MAX=100.0f;
const float G_V=5.0f/3.3f, G_I=5.0f/3.3f, G_G=1.515f, G_T=1.515f;
float readSignal(int pin, float phys_max) {
  uint32_t acc = 0;
  for (int i = 0; i < 20; i++) acc += analogRead(pin);
  float adc_avg = acc / 20.0f;
  float v_adc = (adc_avg / 4095.0f) * 3.3f;
  return (v_adc / 3.3f) * phys_max;
}

const int CTRL_RX=16, CTRL_TX=17;
const unsigned long SAMPLE_MS=500, RETRY_MS=600, ACK_FRESH_MS=300;
mppt::Controller controller(MPPT_LEARNING_ENABLED);
mppt::Decision decision={};
float command_pct=50.0f, confirmed_command=50.0f, applied_pct=50.0f;
uint32_t sequence=0, ack_sequence=0, retries=0;
bool waiting_ack=true, have_ack=false;
unsigned long last_send_ms=0, last_ack_ms=0, last_control_ms=0, last_status_ms=0;
char rx_line[128];size_t rx_length=0;bool rx_overflow=false;
unsigned int chip_mode=0, array_ns=0, array_np=0;
float prev_v=0, prev_i=0, prev_p=0;int direction=1;

void writeCommand() {
  char frame[64];
  snprintf(frame,sizeof(frame),"SET,%lu,%.3f",(unsigned long)sequence,command_pct);
  Serial2.println(frame);
  last_send_ms=millis();
}
void sendCommand(float value) {
  command_pct=mppt::clamp(value,0,100);
  if(++sequence==0)++sequence;
  waiting_ack=true;
  writeCommand();
}
void parseAck() {
  unsigned long seq=0;float cmd=0,applied=0;
  unsigned int mode=0,ns=0,np=0;char extra=0;
  int n=sscanf(rx_line,"ACK,%lu,%f,%f,%u,%u,%u%c",&seq,&cmd,&applied,&mode,&ns,&np,&extra);
  if(n!=6||seq!=sequence||!std::isfinite(cmd)||!std::isfinite(applied)
      ||cmd<0||cmd>100||applied<0||applied>100||std::fabs(cmd-command_pct)>.01f
      ||mode>1||ns<1||ns>12||np<1||np>10)return;
  ack_sequence=(uint32_t)seq;confirmed_command=cmd;applied_pct=applied;
  chip_mode=mode;array_ns=ns;array_np=np;
  have_ack=true;waiting_ack=false;last_ack_ms=millis();
}
void pollAck() {
  while(Serial2.available()) {
    int b=Serial2.read();if(b<0)break;
    if(b=='\r')continue;
    if(b=='\n') {
      if(!rx_overflow&&rx_length){rx_line[rx_length]=0;parseAck();}
      rx_length=0;rx_overflow=false;
    }else if(!rx_overflow){
      if(rx_length+1<sizeof(rx_line))rx_line[rx_length++]=(char)b;
      else rx_overflow=true;
    }
  }
}
void setup() {
  Serial.begin(115200);
  Serial2.begin(115200,SERIAL_8N1,CTRL_RX,CTRL_TX);
  analogReadResolution(12);analogSetAttenuation(ADC_11db);
  Serial.println("# Experimental copy: automatic load control with applied-load ACK. Keep slider Load=50 for no disturbance.");
  Serial.println("t_ms\tV_V\tI_A\tP_W\tG_Wm2\tT_C\tACK_seq\tconfirmed_cmd_pct\tapplied_pct\tnext_cmd_pct\tDelta_cmd_pct\tsource\tref_valid\tVref_control_V\terror_V\tI_pred_A\tmodel_error_A\tlearning_updates\tlearned\tparameter_change\tcontrol_us\tat_limit\tretries\tVmp_nn_V\tVmp_power_V\tq_nn_WperV\tpeak_noise_V\tpower_checks\tprobe_phase\tgradient_valid\tlearning_reason\tlearning_rejections\tloss_before\tloss_after\tverification_kind\tq_measured_WperV");
  // V9: acquisition/UART unchanged; measured-power verification and guarded updates.
  Serial.print("# CONFIG protocol=MPPT_DEV_V9 mode=");Serial.print(MPPT_MODE);
  Serial.print(" online=");Serial.print(MPPT_LEARNING_ENABLED ? 1 : 0);
  Serial.print(" active_verify=");Serial.print(MPPT_ACTIVE_VERIFICATION);
  Serial.println(" sample_ms=500 v_max=50 i_max=15 config_rev=V9_CFGFIX_01");
  Serial.println("# V9 measured-power verification in both modes; guarded current+power-slope neural updates only when online=1");
  Serial.println("# TIMING control_us excludes acquisition, serial output and ACK waiting");
  Serial.print("# EXP_START t_ms=");Serial.println(millis());
  sendCommand(50.0f);
}
float conventional(float v,float i,float p,float load) {
  float step=0;
#if MPPT_MODE == MPPT_PO
  if(p-prev_p<0)direction=-direction;
  step=direction*1.0f;
#elif MPPT_MODE == MPPT_INC
  const float dv=v-prev_v,di=i-prev_i;
  if(std::fabs(dv)<.05f){if(std::fabs(di)>=.02f)step=di>0?-1.0f:1.0f;}
  else if(v>1e-6f){float c=di/dv+i/v;if(std::fabs(c)>=.01f)step=c>0?-1.0f:1.0f;}
#elif MPPT_MODE == MPPT_ADAPT_PO
  const float dv=v-prev_v,dp=p-prev_p;
  float amount=std::fabs(dv)>.01f?mppt::clamp(.08f*std::fabs(dp/dv),.2f,3.0f):.2f;
  if(dp<0)direction=-direction;
  step=direction*amount;
#endif
  (void)v;(void)i;(void)p;
  return mppt::clamp(load+step,0,100);
}
void loop() {
  pollAck();
  unsigned long now=millis();
  if(waiting_ack) {
    if(now-last_send_ms>=RETRY_MS){++retries;writeCommand();}
  }else if(have_ack&&now-last_ack_ms>ACK_FRESH_MS){waiting_ack=true;++retries;writeCommand();}
  const bool ready=have_ack&&!waiting_ack&&now-last_ack_ms<=ACK_FRESH_MS
      &&chip_mode==1&&array_ns==1&&array_np==1&&now-last_send_ms>=100;
  if(!ready) {
    if(now-last_status_ms>=1000){
      Serial.print("# HOLD: ");
      if(!have_ack||waiting_ack)Serial.print("waiting applied-load ACK");
      else if(chip_mode!=1)Serial.print("chip manual mode");
      else if(array_ns!=1||array_np!=1)Serial.print("model requires Ns=Np=1");
      else Serial.print("settling");
      Serial.print("; seq=");Serial.println(sequence);last_status_ms=now;
    }
    delay(1);return;
  }
  if(now-last_control_ms<SAMPLE_MS){delay(1);return;}
  last_control_ms=now;
  const float v=readSignal(PIN_V,V_SENSE_MAX)*G_V;
  const float i=readSignal(PIN_I,I_SENSE_MAX)*G_I;
  const float g=readSignal(PIN_G,G_MAX)*G_G;
  const float t=readSignal(PIN_T,T_MAX)*G_T;
  const float p=v*i;
  const unsigned long start=micros();
#if MPPT_MODE == MPPT_LEARNING
  decision=controller.update(v,i,g,t,confirmed_command,applied_pct);
#else
  decision=mppt::Decision{};decision.load=confirmed_command;decision.source=10+MPPT_MODE;
  decision.load=conventional(v,i,p,confirmed_command);
  decision.delta=decision.load-confirmed_command;
  decision.at_limit=decision.load<=0||decision.load>=100;
#endif
  const unsigned long elapsed=micros()-start;
  Serial.print(now);Serial.print("\t");Serial.print(v,3);Serial.print("\t");Serial.print(i,3);
  Serial.print("\t");Serial.print(p,3);Serial.print("\t");Serial.print(g,1);Serial.print("\t");Serial.print(t,2);
  Serial.print("\t");Serial.print(ack_sequence);Serial.print("\t");Serial.print(confirmed_command,3);
  Serial.print("\t");Serial.print(applied_pct,3);Serial.print("\t");Serial.print(decision.load,3);
  Serial.print("\t");Serial.print(decision.delta,3);Serial.print("\t");Serial.print(decision.source);
  Serial.print("\t");Serial.print(decision.reference_valid?1:0);Serial.print("\t");Serial.print(decision.reference,3);
  Serial.print("\t");Serial.print(decision.error,3);Serial.print("\t");Serial.print(decision.predicted_current,3);
  Serial.print("\t");Serial.print(decision.model_error,3);Serial.print("\t");Serial.print(decision.learning_updates);
  Serial.print("\t");Serial.print(decision.learned?1:0);Serial.print("\t");Serial.print(decision.parameter_change,6);
  Serial.print("\t");Serial.print(elapsed);Serial.print("\t");Serial.print(decision.at_limit?1:0);
  Serial.print("\t");Serial.print(retries);
  Serial.print("\t");Serial.print(decision.nn_reference,3);Serial.print("\t");Serial.print(decision.power_reference,3);
  Serial.print("\t");Serial.print(decision.power_slope,4);Serial.print("\t");Serial.print(decision.peak_noise_V,4);
  Serial.print("\t");Serial.print(decision.power_checks);Serial.print("\t");Serial.print(decision.probe_phase);
  Serial.print("\t");Serial.print(decision.gradient_valid);Serial.print("\t");Serial.print(decision.learning_reason);
  Serial.print("\t");Serial.print(decision.learning_rejections);Serial.print("\t");Serial.print(decision.loss_before,5);
  Serial.print("\t");Serial.print(decision.loss_after,5);Serial.print("\t");Serial.print(decision.verification_kind);
  Serial.print("\t");Serial.println(decision.measured_slope,4);
  prev_v=v;prev_i=i;prev_p=p;
  sendCommand(decision.load);
  delay(1);
}

# Correspondencia entre algoritmo, LaTeX y fuente evaluada

| Elemento del manuscrito | Fuente contrastada | Comprobación |
|---|---|---|
| 2.5.1: entrada y estado persistente | Controller.reset/update en mppt_neural.h; setup y parseAck en sketch.ino | 17 parámetros de salida, memoria válida, contador, comando inicial50 y reconocimiento del comando. |
| 2.5.2: normalización y entrenamiento | mppt_aprendizaje_v5/tools/train.py; Controller.features/predict | Entradas (V−21)/21 y (T−37.5)/12.5, target I/(G/1000)/14, 16 tanh, solver lbfgs, alpha1e−4, seed20261026, max_iter5000, max_fun150000. |
| 2.5.3: admisión de datos y dominio | loop/pollAck/parseAck; Controller.update | Periodo500ms, ACK300ms, espera100ms, reintento600ms; dominio G399–801 y T24.9–50.1; clip al rango de entrenamiento. |
| 2.5.4: actualización NLMS | Controller.update/normalized/distance | Error físico antes de aprender; residuo normalizado crudo; umbral0.04A, tasa0.10, residuo±0.10, proyección respecto a theta0±0.25, contador y distancia. |
| 2.5.5 y Algoritmo2: búsqueda | Controller.target/predict | U=clip(39.75−1.55(T−37.5)/12.5,1,45), 101 candidatos y 21 de refinamiento; conserva el mejor punto grueso. |
| 2.5.6: regulación | Controller.update | Zona muerta0.15V, ganancia0.5pp/V, paso±3pp, comando0–100%, base de corrección confirmada. |
| Algoritmo2: respaldo | Rama exterior de Controller.update | Inicialización, dV0.05V, dI0.02A, umbral de conductancia0.01A/V, guarda V>1e−6 y pasos±1pp. |
| Algoritmo1: secuencia completa | sketch.ino y Controller.update | No invoca control mientras falla admisión; medición, predicción, aprendizaje opcional, búsqueda, regulación, memoria, telemetría y nuevo comando. |

La figura de mérito de la identificación es un residual de corriente. La energía se mide por separado. La actualización no equivale a una política entrenada directamente para maximizar energía. La descripción no atribuye el entrenamiento de pesos a comandos históricos no usados por la versión desplegada.

# Auditoría de reproducibilidad del estudio MPPT neuronal

**Base científica:** manuscrito de 34 páginas. **Alcance:** simulaciones host y simulaciones de firmware en Wokwi; sin ensayos físicos de convertidores. **Estado del depósito:** borrador, sin DOI.

## Inventario de evidencia

Se preservaron los **1666 archivos** del paquete de datos y los **7 archivos** descargados del proyecto Wokwi. Se registraron las huellas SHA-256 de **1673 archivos**. Se encontraron las cuatro curvas I–V utilizadas para calibración inicial (1012 observaciones), los desarrollos V6, V8 y V9, las herramientas de análisis y los registros experimentales.

| Prueba | Archivos | Comprobación |
|---|---:|---|
| Campaña host inicial | 350 | Compilada y ejecutada de nuevo |
| Comparación host de algoritmos clásicos | 330 | Compilada y métricas iguales a las archivadas |
| Comparación host de referencia afín | 110 | Compilada y métricas iguales a las archivadas |
| Comparación host neuronal NN-C/NN-V | 440 | Compilada, con algunas diferencias internas |
| Ablación pasiva host NN-V | 60 | Compilada y métricas iguales a las archivadas |
| Archivos Wokwi | 29 | 7491 observaciones completas, 2 fragmentos incompletos excluidos |
| Diagnóstico por entradas idénticas | 32 combinaciones | Script ejecutado correctamente |

La comparación ampliada puede recalcularse con `supplementary/matched_extension/prepare_article_data.py`. Las cifras de eficiencia de la tabla principal de S0–S4 coinciden, al redondeo publicado, con los valores almacenados.

## Limitaciones identificadas

1. En la campaña inicial, 31/350 registros no son idénticos byte por byte en una nueva ejecución; la eficiencia agregada NN-I online fue aproximadamente 99.42736847 % en el archivo original y 99.42737843 % al ejecutar nuevamente.
2. En la comparación neuronal, 82/440 registros de métricas tienen al menos una diferencia. Tres incluyen variaciones pequeñas de eficiencia. Algunos contadores de actualizaciones aceptadas y rechazadas de NN-V online difieren de forma mayor. La causa **no está determinada**, aunque los SHA-256 del firmware y del chip de referencia coinciden.
3. Falta el programa original que produjo el entrenamiento inicial de los 65 parámetros NN-I/V6. Sí están disponibles los pesos resultantes y el programa de recalibración NN-C.
4. Faltan el archivo binario del ESP32, el componente solar WASM compilado y un entorno de compilación histórico bloqueado por versiones. El proyecto Wokwi descargado contiene los fuentes. El enlace debe probarse desde una sesión con acceso web antes de publicar.
5. Las trazas Wokwi históricas no identifican de forma concluyente el binario que produjo cada archivo; no deben describirse como una validación física ni como una campaña final NN-V emparejada.

## Estado de publicación

No depositar aún en Zenodo como versión definitiva. Falta decidir licencias y autorización de los autores, resolver los puntos anteriores o declararlos como limitaciones, comprobar el enlace público de Wokwi y generar la versión citada con DOI. La documentación del paquete indica las rutas de ejecución y conserva los datos originales, sin alterarlos.


### Actualización de la auditoría: entrenamiento recuperado

La ausencia del script inicial indicada arriba ha quedado **resuelta** con el ZIP V5 adjuntado posteriormente. `tools/train.py` reprodujo sin diferencias binarias los 65 pesos y el informe del entrenamiento. Los demás pendientes (contadores NN-V, binarios y versiones de herramientas de Wokwi) permanecen abiertos.

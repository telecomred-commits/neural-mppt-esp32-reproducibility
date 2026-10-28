# Matriz de cambios — versión 8

Base: ZIP canónico versión 6 de archivo, manuscrito V7. Identidad y checksum en base_identity.json. Solicitud del autor: actualizar resultados y analizar ventajas adicionales al seguimiento de potencia.

| Solicitud o hallazgo | Decisión aplicada | Localización final | Verificación |
|---|---|---|---|
| Actualizar con toda la evidencia disponible | Distinguir 29 archivos Wokwi/7491 filas, 350 ensayos iniciales, 880 emparejados, 60 de ablación y 32 reproducciones diagnósticas | Resumen; 2.6; 3.1–3.6 | Conteos y hashes en suplemento; 940 trazas de extensión verificadas |
| No confundir versiones del controlador | Definir NN-I/V6, NN-C/V8 y NN-V/V9; modalidades FROZEN/ONLINE separadas | 2.5.7, Tabla 4, p.12 | Correspondencia con cabeceras y pesos congelados |
| Centrar materiales y métodos en el algoritmo | Documentar calibración, proyección, ajuste de potencia, aceptación y parámetros; incluir pseudocódigo NN-V | 2.5.7–2.5.9, pp.12–15; Algoritmo 1 | Comparado con código y pruebas de contrato; pseudocódigo NN-I archivado |
| Comparar bajo los mismos controles | Completar 330 ensayos clásicos y 110 afines con perfiles/semillas de la candidata sin resintonizarla | 2.6.9; 3.4; Tabla 11 | 880 métricas; agregados recalculados por semilla |
| Separar calibración de representación neuronal | Comparador afín derivado de los mismos máximos medidos | Ec.32; Tabla 11; Discusión 4.2 | Afín 99,4295 % frente a NN-C 99,4302 % |
| Analizar beneficios adicionales | Medir variación, actividad y recuperación; distinguir capacidades de identificación y coste | 3.5, Tabla 12, Figura 4, pp.27–28; 4.2, Tabla 13, pp.29–30 | Ventanas y denominadores explícitos; sin atribuir pérdidas o desgaste físicos |
| No sobredimensionar el aprendizaje | Mantener ausencia de mejora práctica; diagnóstico de menor residuo con mayor error de referencia | 3.4–3.6; Figura 5; 4.1; Conclusiones | Reproducción de entradas idénticas; coste de verificación y fallos R1/H0 conservados |
| Conservar modelo/comparación comercial aprobados | Mantener Tabla 2 con diez fichas, figura al final de 2.3 y artículo de Energies | pp.6–7; bibliografía | Figura con checksum idéntico; Tabla 2 conserva numeración y citas |
| Actualizar relación con literatura | Conservar matriz de diez estudios y añadir antecedente híbrido ANN/INC de Chen y Wang | 4.3, Tabla 14, pp.31–32; ref.42 | DOI/editorial comprobados; registro en new_reference_v8.json |
| Entregar fuente y PDF coherentes | Master modular y suplemento trazable, PDF 37 páginas | Paquete V8 completo | XeLaTeX/XDV; 42 referencias; sin citas/refs pendientes; revisión visual de todas las páginas |

No se remitió el manuscrito a la revista. Autoría y declaraciones permanecen pendientes de confirmación. Las advertencias de clase/fontes heredadas no impiden la compilación; no hay cajas Overfull ni referencias indefinidas en el log final.

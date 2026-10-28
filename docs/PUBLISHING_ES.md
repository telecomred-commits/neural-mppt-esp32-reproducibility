# Publicación del repositorio — instrucciones para los autores

El paquete se entrega listo para ser revisado y cargado como un repositorio GitHub. **No se ha publicado ni tiene DOI.** El manuscrito de 34 páginas no se modifica.

1. Revisar que los archivos sean propios o que exista autorización para su redistribución. Esto incluye notas, material histórico y proyectos derivados; no incluir archivos de terceros sin permiso.
2. Elegir una licencia para el código y otra para los datos. Las licencias no se han asignado de manera unilateral; `LICENSE_PENDING.md` explica el estado.
3. Comprobar la apertura pública del proyecto https://wokwi.com/projects/479082581022049281 y congelar la copia correspondiente al envío de la revista.
4. Ejecutar `python tools/preflight.py` en una copia recién descomprimida y guardar el resultado.
5. Crear un repositorio público de GitHub, subir el contenido del directorio raíz de este paquete, revisar el `README.md` y aprobar la versión con ambos autores.
6. Crear una versión de publicación (*release*) con identificador estable. Archivar ese lanzamiento en Zenodo para obtener un DOI de versión, y comprobar que el archivo archivado corresponde al repositorio mediante SHA-256.
7. Incorporar en el manuscrito, al final, el enlace de Wokwi, la URL del repositorio, el DOI y la declaración de disponibilidad de datos; no inventar ningún identificador.

Los scripts de simulación pueden sobrescribir resultados generados. Para repetir campañas se recomienda duplicar el repositorio y trabajar exclusivamente en esa copia.

Cripto-2014
===========

TPE 2 Criptografía y Seguridad

Para la correcta ejecución del programa se deberan tener instaladas las librerías de argtable y openssl.
Las librerías de argtable se encuentran en la carpeta actual bajo el nombre argtable2-13, para su instalación, ejecutar los siguientes comandos:

    $> cd argtable2-13
    $> ./configure
    $> sudo make install
    $> make clean

Luego de tener dichas librerías instaladas, se compila el programa usando el comando 'make', lo cual genera un archivo ejecutable bajo el nombre de 'shared_secret'

Para ejecutar el mismo, usar el comando ./shared_secret desde la consola con los siguientes parámetros:

    Parametros obligatorios:
        -r o -d: Indica que se desea recuperar (-r) o distribuir (-d) una imagen.
        -k <número>: Indica la cantidad de sombras a generar/usar para la distribución/recuperación de la imagen (solo válidos valores 2 o 3).
        -s <archivo>: Nombre del archivo para buscar la imagen en caso de distribución o dejar la imagen recuperada en caso de recuperación.

    Parametros optativos:
        --dir <directorio>: Directorio de donde obtener las imágenes para la distribución/recuperación.
        -n <número>: Cantidad de sombras a generar.

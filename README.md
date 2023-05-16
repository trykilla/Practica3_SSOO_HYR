# Practica3_SSOO_HYR

Práctica realizada por los alumnos de **Sistemas Operativos II** de tercer curso de Rubén Crespo Calcerrada y Héctor Alberca Sánchez-Quintanar, el objetivo de esta práctica es crear una serie de clientes, que pueda haber 3 tipos de **clientes**, todos estos clientes buscan una palabra aleatoria dentro de un diccionario, esta palabra será buscada en una serie de ficheros. Cabe destacar que la búsqueda se hace a través de **varios hilos** en un único archivo, es decir, recorrerán todos los archivos uno a uno siendo divididos en una cantidad determinada de hilos. El sistema de pago será otro hilo que esperará hasta que un cliente premium quiera pagar.

A través de nuestro programa se llevan a cabo la creación de una serie de hilos que serán los clientes, cada uno de estos clientes podrá buscar una palabra determinada, dependiendo de cada cliente se hará de una determinada manera u otra. Los buscadores serán otros hilo cada uno perteneciente a un determinado cliente y por último tenemos el buscador que será otro hilo. 

## MODO DE EJECUCIÓN.

Para ejecutar este código se ha llevado a cabo un Makefile para la realización más sencilla. Lo primero que debemos hacer es ejecutar el comando *"make"* dentro del directorio, de esta manera se compilarán todos los archivos necesarios para su ejecución, si hacemos algún cambio también debemos hacerlo para actualizar la compilación. 

A continuación, debemos hacer un *“make clear_file”* para borrar el archivo generado en la última ejecución del programa. Después, se lleva a cabo la ejecución de la instrucción *"make test"*, así se llevará a cabo la ejecución del programa, generándose tanto la impresión en pantalla de determinada información y añadiéndose los resultados encontrados en el *“Results.txt”*
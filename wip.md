# Backend

[X] Scroll bien hecho (teniendo en cuenta los margenes) -> necesito implementar blit de buffer a buffer (en modo gráfico) y complicar un poco la copia en modo texto.
[X] Añadir clicks a PAD_THIS_FRAME y usarlo en los menús y zonas!

# Intérprete

[X] Legacy Lbasic 6
[ ] Legacy Lbasic 7
[/] Legacy Lbasic 8
[ ] GOSUB
[X] Autogo
[X] Inventario sencillo ~ parecido a menús, añadiendo Tokens a una lista
[X] Bulma "draw" support (LIN)
[ ] caracter parpadeante en pulsa una tecla: indicator off|on x y c1 c2 c
[X] Bulma stuff
	- actions reset
	- actions put X
	- actions remove X

	- zones reset
	- zones def x1 x2 y1 y2 name [items|actions]
	- zones run :label

	:label_name / :label_name_item / :label_name_action

[ ] Controlar ESC en menús y zones para salir del juego <- reaprovechar TUI -- parametrizar cuadro de texto para que lo centre en cualquier resolución de pantalla!!! -> Ejecutar spec.END, si existe. Si no, salir.
[X] Flujo chain "file.ext", sin restricción. Se le añade "directory spec" al principcio.como a todo.
[X] Caja de texto bulma
[X] Itegración "EXTERN N M" para que lo implemente el juego que sea
[-] Añadir un flag para que los comandos no esperen vbl, por lo general!
[ ] print con N parámetros incluyendo flags `print "El valor introducido es " %perro " !"`
[/] cajas de "talk" con 3 opciones para clickear / menus grandes  que generan 1 2 3 [UNTESTED]
[X] Añadir parámetros a fancy cls para eliminar solo una zona de la pantalla y no toda.
[ ] Añadir fancy a PIC / CUT ?
[ ] Formas de contar el número de elementos que hay en el menú, o el número de elementos que se ha quitado desde el último reset!
[ ] Box manual
[X] Menu config noframe
[ ] Talk config retratos, funciona así:

`portraits config x y ["prefijo"]` activa los retratos, que se cargarán de `prefijo` .

`wwc "text" ["personaje" ["sufijo"]]`

En el `wwc`, si los retratos están activados, se carga "prefijo/personaje.gif", con "personaje" el que se especifica como segundo parámetro de `wwc`. Si se añade un tercer parámetro, se cargará "prefijo/personaje_sufijo.gif", con `parametro` el valor que se especifique. 

`portraits off` los desactiva.

Por ejemplo

```
	portraits config 260 160 "pics/portraits/"
	wwc "Se está muy bien aquí" "Xinkss" "2"
```

Mostrará el retrato en `pics/portraits/Xinkss_2.gif`.

[X] Hay un problema con los flags y los aliases y tal, deberia distinguirse entre lvalue y rvalue.

Por ejemplo:
add $4 $5 -> 
$4 = $4 + $5
flags [4] = flags [4] + flags [5];

En la parte derecha puedo dejarlo tal y como está, resolviendo,
Pero en la parte izquierda tendría que obtener "4".

inc %cojin -> inc $2, idem,
inc $2 -> necesito obtener "2"

let $4 = 4, es más legible, necesito "4" (lvalue)
let %cojin = 5, es más legible.

¡Implementar bien esto!
La función que hay que resuelve flags me sigue valiendo para "rvalue", pero no para "lvalue".

[ ] Control de la salida.

GOAL: Poder detectar cuando el usuario quiere salir para mostrar un menú desde que el usuario pueda ofrecer al jugador salir sin más o grabar la partida.

Lo suyo es que se saltara aun bloque especial (.999 por ejemplo, o .end, o configurable) al detectarse un ESC en determinados momentos o cuando se pulsa el X de la ventana.

Problema 1: Cuando saltar.
Problema 2: Dónde volver.

Sobre 1:
	* ESC en un submenú debería interrumpir el menú.
	* ESC en el menú principal - SI
	* ESC en "selecting zones" - SI
	* ESC en menú zones debería interrumpir el menú.
	* ESC en WT/Pause - SI

	* Necesito modificar el dos-like para poder controlar completamente la detección de pulsar en X de la ventana - poder desactivar el estado por ejemplo si se decide no salir. --- `resetshuttingdown`

Sobre 2:
	* Mi primera idea es añadir un comando especial "LOC" que decide cuando empieza la localización actual y que funciona como una etiqueta dinámica a la que se volverá al terminar 999 si no hay un END explícito o al escribir RETLOC.

[X] PROBLEMA: He puesto hostias para resetear el flag interno de pulsar "X" en dos-like pero parece que no sirvió para nada, internamente lanza sus historias para cerrar ? -> resuelto

[ ] Tengo que hacer que ir a 999 sea más seguro:
	[ ] guardar y restaurar las opciones del menu
	[ ] guardar y restaurar las zonas de pantalla
	[ ] guardar y restaurar la pantalla

[ ] SAVE / LOAD state - graba todo: 
	* Bloque actual (#).
	* Nombre de archivo ejecutándose.
	* Posición de LOC.
	* Todos los flags.
	* Todos los items.

	Cargar un estado carga bloque actual y salta a LOC.
# Editor

[X] Save / Load
[ ] wizards
	[ ] Choice 
	[ ] Choigo 
	[ ] Go 
	[X] Print 
	[ ] Color 
	[ ] TUI Based for stuff with many parameters
[X] RUN / RUN FROM / NEW
[X] Editor DRAW
[ ] Editor bulma draw (LIN)
[ ] Editor zones->código
[ ] Auto-generar etiquetas para zones->run
[/] AV PAG / RE PAG / HOME / END / CTRL + arrows
[X] A veces se salva mal (creo que mezcla `\n` con `\r\n`), revisar. Cuando cargas->editas->grabas. En HEX EDITOR veo DOS \r... -> Grabar en binario
[X] Syntax no saca los textos entre comillas hasta que se cierran
[ ] Hacer que el ratón funcione en el menú superior

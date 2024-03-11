# LBasic Commands

- cursor|setxy x y
- print "what"
- write "what"
- center "what"
- color c1 c2
- pause
- beep
- cls
- draw "what"
- choice n c "what1" ...
- viewport l1 l2
- attempts n
- statusbar on|off
- margins c1 c2
- ww "what" [cbc]
- ansibin "pic.bin"
- let f v
- input f
- tell f
- inc f
- dec f
- add f v
- sub f v
- go :label
- eq v1 v2 :label
- neq v1 v2 :label
- ge v1 v2 :label
- lt v1 v2 :label
- autogo f :label
- mode text|gfx|gfx_sq|gfx_med|gfx_hi
- pic "pic.gif" seconds|kbd
- cut "cut.gif" x y
- lin "pic.lin"
- sleep seconds
- menu
- items
- talk
- zones
- indicator
- interface "spec.gif"

## cursor|setxy x y

```
	cursor|setxy x y
```

Establece la posición de escritura en x, y

## print

```
	print "texto"
```

Imprime un texto tal cual, con un salto de linea al final. "Tal cual" significa que saca a pantalla exactamente lo que tiene, sin importar que se corte alguna palabra. Es el programador quien debe ocuparse de que esto no ocurra. Tras imprimir pasará a la linea siguiente.

## write

```
	write "texto"
```

Imprime un texto tal cual. "Tal cual" significa que saca a pantalla exactamente lo que tiene, sin importar que se corte alguna palabra. Es el programador quien debe ocuparse de que esto no ocurra. Tras imprimir no hará nada más, dejando el cursor donde se haya quedado.

## center

```
	center "texto"
```

Imprime un texto tal cual, centrado en pantalla, con un salto de linea al final. El comportamiento no está definido si se intenta centrar un texto que no quepa en la linea, así que cuidado. Tras imprimir pasará a la linea siguiente. 

## color

```
	color c1, c2
```

Sirve para cambiar el color del texto. 'c1' es el número del color para las letras, 'c2' es el color del fondo. Si se pasa -1 como cualquiera de los parámetros no se actualizará este valor. En los modos gráficos, si `c2` vale `256` o más el texto se imprime sin color de fondo.

## pause

```
	pause
```

Interrumpe la ejecución hasta que el usuario pulse una tecla.

## beep

```
	beep
```

Produce un pitido corto

## cls

```
	cls
```

Borra la zona de texto de la pantalla (ver `viewport` y `margins`);

## fancy_cls

```
	fancy_cls [x1 y1 x2 y2]
```

Pensado para modos gráficos. Sin parámetros borra toda la pantalla con un bonito efecto. Con parámetros borra sólo un área. Las coordenadas van en pixels y en teoría deberían comprender un área de ancho y alto múltiplo de 8 (en baja resolución) o 16 (en alta resolución) pixels, por ejemplo `0 0 255 191`

## draw

```
	draw
```

[Legacy] (TODO)

## choice

```
	choice N C "text1" "text2" ...
```

[Legacy] Espera que el usuario introduzca un número entre `1` y `N`. Luego presenta el texto correspondiente `textN`. Si el número corresponde con `C`, se considera que se eligió la opción correcta. Si no, el motor restará un reintento al jugador y actuará en consecuencia.

Por ejemplo:

```
	choice 3 1 "El usuario introdujo 1. Esta es la opción correcta" "El usuario introdujo 2, game over" "El usuario introdujo 3, game over"
```

## viewport

```
	viewport l1 l2
```

Establece la zona de texto desde la linea l1 a la linea l2, ambas inclusive. La primera linea de la pantalla es la 0. Por defecto, el motor establece la pantalla completa en los modos gráficos o toda la zona central excluyendo una linea arriba y una abajo para los modos de texto (valores 1, 23).

## attempts

```
	attempts N
```

(Re)establece a `N` el número de veces que el usuario puede fallar un `choice`.

## statusbar

```
	statusbar on|off
```

Permite desactivar la linea de estado que aparece en modo de texto.

## margins

```
	margins c1 c2
```

Establece el margen izquierdo y derecho para la impresión tipo "word wrap" (ver `ww`). El margen izquierdo de la pantalla equivale a 0, el derecho depende del modo. Ambos valores son inclusive. El ancho completo en la pantalla de texto sería `0 79`, por ejemplo, y en los modos gráficos de baja densidad sería `0 39`.

## ww

```
	ww "texto" [cbc]
```

Escribe un texto con "word wrapping" y pasa a la linea siguiente. Este método ajusta las palabras del texto entre las columnas definidas en margins, de modo que si una palabra no cabe en una linea pasará a la siguiente. Es imporante emplear espacios tras la puntuación para que funcione de forma óptima. Si se añade `cbc` el texto se muestra caracter a caracter con un delay (interrumpible al pulsar una tecla).

```
	wwc "texto" ["personaje"]
```

Alias de `cls; talk "personaje"; ww "texto" cbc; pause; cls` que además restaura la pantalla. Pensado para visual novels.

## ansibin

```
	ansibin "pic.bin"
```

Carga una imagen en formato ansi del mismo directorio donde está el script que se está ejecutando y la muestra en pantalla. Las imagenes deben tener un formato binario análogo al de la memoria de video en modo texto. Sólo puede usarse en modo de texto.

Este tipo de imagenes puede generarse con editores de texto Ansi como "Moebius" y funcionarán siempre que se exporten en formato binario.

## let

```
	let flag valor
```

Asigna a la flag `flag` el valor `valor`. `flag` debe resolver a un número entre 0 y 255, o sea, algo com `$7` o `#gato`. Ver la sección sobre flags.

LBasic soporta alias de forma transparente. Los alias se asignan a un número de flag y le dan una forma más amable de llamarlos. La asignación es automática:

```
	let %alias valor
```

Creará `%alias` y le asignará valor al flag asociado. Para referenciar este flag (del que no sabemos el número) usaremos la forma `%alias`. Por ejemplo:

```
	let %perro 5
	tell %perro
```

Escribe 5. Internamente, habrá asignado 5 a un flag libre. Mezclar flags numéricos con alias es generalmente mala idea a menos que sepas qué estás haciendo.

**Esta es la únia forma de crear aliases de flags**. Para usar en un input antes deberá ser creado con let, aunque se le asigne un valor que luego jamás vaya a usarse. **Los aliases podrán tener hasta 32 caracteres**, si son más largos no se registrarán y el script no funcionará.

## input 

```
	input flag
```

Asigna a la flag `flag` el valor que introduzca el usuario. `flag` debe resolver a un número entre 0 y 255, o sea, algo com `$7` o `#gato`. Si se usa un alias (`#gato`) este debe estar ya creado con `let`. Ver la sección sobre flags.

## tell

```
	tell flag
```

Escribe en pantalla el valor de la flag `flag` y deja el cursor donde está. `flag` debe resolver a un número entre 0 y 255, o sea, algo com `$7` o `#gato`. Ver la sección sobre flags.

## incc

```
	inc flag
```

Incrementa en 1 el valor de la flag `flag`. `flag` debe resolver a un número entre 0 y 255, o sea, algo com `$7` o `#gato`. Ver la sección sobre flags.

## dec

```
	dec flag
```

Decrementa en 1 el valor de la flag `flag`. `flag` debe resolver a un número entre 0 y 255, o sea, algo com `$7` o `#gato`. Ver la sección sobre flags.

## add

```
	add flag valor
```

Suma `valor` al valor de `flag` y lo almacena en `flag`. `flag` debe resolver a un número entre 0 y 255, o sea, algo com `$7` o `#gato`. Ver la sección sobre flags.

## sub

```
	sub flag valor
```

Resta `valor` del valor de `flag` y lo almacena en `flag`. `flag` debe resolver a un número entre 0 y 255, o sea, algo com `$7` o `#gato`. Ver la sección sobre flags.

## go

```
	go :etiqueta
```

Continua la ejecución a partir de la linea con `:etiqueta`.

## eq, neq, ge, lt

```
	eq v1 v2 :etiqueta
	neq v1 v2 :etiqueta
	ge v1 v2 :etiqueta
	lt v1 v2 :etiqueta
```

Continuan la ejecución a partir de la linea con `:etiqueta` si `v1 = v2`, `v1 <> v2`, `v1 >= v2` y `v1 < v2`, respectivamente. Ver la sección sobre flags.

## autogo

``` 
	autogo flag :label
```

Salta a la etiqueta `:label_F`, con `F` el valor de `flag`. Por ejemplo, puede usarse después de `input`. Recordemos además que si `:label_F` no existe, esta orden no hace nada.:

```
	print "Introduce 1 o 2"

	input 2
	autogo 2 :etiqueta
	go :fin

	:etiqueta_1
	print "El usuario introdujo 1"
	go :fin

	:etiqueta_2
	print "El usuario introdujo 2"

	:fin 
```

## mode

```
	mode text|gfx|gfx_sq|gfx_med|gfx_hi
```

Establece el modo gráfico / texto:

- `text`: Modo de texto de 80 x 25 caracteres. No funcionará la carga de imagenes GIF (`pic` o `cut`).
- `gfx`: Modo de gráficos de 320 x 200, con 40 x 25 caracteres. No funcionará la carga de imagenes ANSI.
- `gfx_sq`: Modo de gráficos de 320 x 240, con 40 x 30 caracteres. No funcionará la carga de imagenes ANSI.
- `gfx_med`: Modo de gráficos de 640 x 200, con 80 x 25 caracteres. No funcionará la carga de imagenes ANSI.
- `gfx_hi`: Modo de gráficos de 640 x 350, con 80 x 43 caracteres. No funcionará la carga de imagenes ANSI.

## pic

```
	pic "pic.gif" seconds|kbd
```

Carga la imagen `pic.gif` a pantalla completa (la imagen debe tener un tamaño adecuado conforme al modo gráfico) y espera `seconds` segundos (puede ser un valor decimal) o a la pulsación de una tecla, según se especifique un valor o `kbd`:

```
	pic "mediosegundo.gif" 0.5
	pic "pulsatecla.gif" kbd
```

Es equivalente a "PIC" de PRESENTS.EXE (Dr. Genius).

## cut

```
	cut "pic.gif" x y [t]
```

Carga la imagen `pic.gif` en las coordenadas x, y. Si se especifica `t` se utilizará como pixel transparente.

Es equivalente a "CUT" de PRESENTS.EXE (Dr. Genius).

## bg

```
	bg config x y
```

Configura para que los "fondos" que se carguen con "bg" aparezcan siempre en `x, y`.

```
	bg "pic.gif"
```

Carga un fondo `pic.gif` y lo muestra en pantalla en laas coordenadas que se configuraron. Si se configuró `interface` (ver más abajo), se mostrará este interface encima de la imagen cargada.

## lin

```
	lin "pic.lin"
```

Carga una imagen vectorial de pantalla completa en formato Bulma Lin.

## sleep

```
	sleep seconds
```

Detiene la ejecución `seconds` segundos (puede ser un valor decimal) o hasta que se pulse una tecla.

## Valores y flags

## Menu

Estos son los comandos que controlan los menús de opciones con GOTO automático.

```
	menu reset|limpia
```

Borra todas las opciones del menú.

```
	menu add|pon "texto" [:custom_label]
```

Añade al menú la opción con texto `texto`. Si se añade una etiqueta, elegir esta opción saltará a esa etiqueta en lugar de comportarse más abajo (ver `menu run` y la sección de inventario).

```
	menu remove|quita "texto"
```

Quita del menú el la opción con texto `texto`

```
	menu config x y w c1 c2 [fixed] [noframe]
```

Configura el menú para que aparezca en (x, y) con los colores (c1, c2), de anchura w. Si se especifica `fixed` al final, el menú se quedará en pantalla tras seleccionar uná opción en lugar de desaparecer. Esto es por si quieres tener el menú fijo en pantalla. En este caso, puedes / tendrás que ejecutar:

```
	menu show
```

Cada vez que borres la pantalla o la cubras con una imagen nueva, o hagas cualquier cosa que pueda sobrescribir el menú. **En otro caso no es necesario**.

Si pones `noframe` no se pintará un marco alrededor del menú y el texto de las opciones podrá ocupar todo `w` de ancho.

```
	menu run :label :label_ret
```

Muestra el menú y deja al usuario elegir una opción, y luego:

- Si el usuario interrumpió el menú (ESC / 99 / etc) no hace nada! 
- Si el usuario eligió una opción, saltará a la etiqueta :label_opcion (*) (+) 
- ¡Ojo! si la etiqueta no existe, no hará nada.

(*) Se sustituirá los espacios por `_`, así una opción `COGER CAJA` saltará a `:label_coger_caja`
(+) en las etiquetas no importa mayúsculas o minúsculas. Mejor no poner acentos (por ahora)

- :label_ret especifica a qué etiqueta se salará con la orden "ret". Es util para volver a la linea que despliega el menú tras realizar una de las acciones.

Ejemplo con rayitas de colores:

![Ejemplo menus](https://github.com/mojontwins/LBasic/blob/master/docs/lbasic_menu.png?raw=true)

```
	menu has|tiene "item" :etiqueta
```

Salta a `:etiqueta` si `item` está en el menu.

```
	menu hasnt|notiene "item" :etiqueta
```

## Items

Estos son los comandos que sirven para llevar un inventario

```
	items reset|limpia
```

Elimina todos los items

```
	items size|tam X
```

Establece a X el valor máximo de objetos que se puede llevar en el inventario.

```
	items add|pon "item"
```

Añade "item" al inventario.

```
	items remove|quita "item"
```

Quita "item" del inventario.

```
	items has|tiene "item" :etiqueta
```

Salta a `:etiqueta` si `item` está en en inventario.

```
	item hasnt|notiene "item" :etiqueta
```

Salta a `:etiqueta` si `item` NO está en el inventario.

## Menú con items

```
	menu add|pon "texto" items
```

Añade una opción al menú con `texto` que, al seleccionarla, desplegará un menú con los items. La etiqueta generada finalmente por `menu run :label` será `:label_usar_ITEM`, con `ITEM` el item que se seleccione en el segundo menú.

## Menú con salidas

```
	exits reset|limpia
```

Limpia las salidas

```
	exits put|pon "texto" :etiqueta
```

Añade una salida con texto `texto` que llevará a `:etiqueta`.

```
	exits remove|quita "texto"
```

Quita la salida con texto `texto`.

```
	menu pon|put "texto" exits
```

Añade una opción al menú con `texto` que, al seleccionarla, desplegará un menú con las salidas. La etiqueta generada finalmente será la etiqueta asociada a la salida.

## Talk

Una pequeña utilidad para cuando hagas visual novels. Puedes poner quien habla fuera de la zona de texto que hace scroll, lo que viene muy bien

```
	talk "personaje"
```

Escribe `personaje`. Puedes usar una cadena vacía.

```
	talk config x y w c1 c2 [ovl.gif ox oy om]
```

Configura el texto para que aparezca en (x, y) con los colores (c1, c2), de anchura w.

Si se especifican los cuatro parámetros entre comillas, se cargará ovl.gif en (ox, oy) con máscara om antes de imprimir. Sirve para hacer marcos bonitos.

## Zones

Las zonas permiten implementar point'n'click sencillo. Las zonas utilizan los menús. La posición (x, y) especificada en `menu_config` se ignorará, sólo se establecerá el color.

Las zonas son rectángulos dentro de la pantalla. Las zonas emplean coordenadas reales: en modo texto se refieren a la rejilla de caracteres. En los modos gráficos las coordenadas son de pixels.

Hay dos tipos de zonas: las sencillas y las que llevan asociada una acción. 

Cuando se ejecuta `zones run :label`, si el usuario hace click sobre una zona sencilla se genera un `go :label_Z`, con `Z` el texto de la zona.  Si el usuario hace click sobre una zona asociada a una acción, se mostrará el menú de acciones. Al pulsar sobre una acción, se genera un `go :label_ACTION_Z` con `ACTION` la acción pulsada y `Z` el texto de la zona. Los espacios se sustituyen por `_` y no importa mayúsculas o minúsculas.

```
	actions reset|limpia
```

Borra todas las acciones.

```
	actions put|pon "texto" [items]
```

Añade una acción con texto "texto". Si se pone "items" al final, la acción mostrará un segundo menú, el de items. En este caso se generará `go :label_ACTION_Z_ITEM`, con `ACTION` la acción pulsada, `Z` la zona e `ITEM` el item elegido.

```
	actions remove|quita "texto"
```

Quita la acción

Normalmente para un juego las acciones se establecen al principio y no cambiarán: examinar, hablar, usar...

```
	zones reset
```

Borra todas las zonas 

```
	zones def "texto" x1 y1 x2 y2 [actions|:label]
```

Añade una zona con `texto` representando al rectángulo que va desde (x1, y1) hasta (x2, y2). Si se especifica `actions` esta zona desplegará el menú de acciones. Si, en cambio, se pone una etiqueta, el juego saltará directamente a esa etiqueta.

```
	zones run :label :label_ret
```

Se espera a que el jugador seleccione una zona y desplegará los menús encadenados que corresponda. La segunda etiqueta es adónde se volverá al ejecutar "ret".

```
	infobar off|y c1 c2
```

Desactiva o activa (y configura) una barra de información que se deplegará mientras el usuario se mueve por la pantalla con el ratón, mostrando el texto de la zona activa.

## En resumen

Para entender los saltos que produce `zones run` veamos un ejemplo con todas las opciones:

```
	item puts "Calabaza"

	actions put "Mirar"
	actions put "Usar" items

	zones def "Cuadro" 40 40 80 80
	zones def "Ir al salón" 0 180 319 199 :salon
	zones def "Cesta" 100 40 140 80 actions

	:prueba_run

	# El prefijo es ":prueba". Todos los saltos usarán ese prefijo.
	# `ret` volverá a ":prueba_run"
	zones run :prueba :prueba_run

	# Si no pulsas en ningun sitio correcto o no hay nada para esa accion
	# el script sigue ejecutandose, así que tenemos que volver a prueba_run
	ret
```

* Si el usuario pulso en la zona entre (40, 40) y (80, 80), o sea, la zona "Cuadro", se saltará a `:prueba_cuadro` (el prefijo en `zones_run` y el nombre de la zona)
* Si el usuario pulsó en la zona asociada a "Ir al salón" (entre (0 180) y (319 199)), se saltará a `:salon`
* Si el usuario pulsó en la zona "Cesta" (entre (100, 40) y (140, 80)), se mostrará el menú de acciones:
> * Si el usuario elige "mirar" se saltará a `:salon_cesta_mirar` (prefijo, zona, accion).
> * Si el usuario elige "usar" se mostrará el menú de items. Si el usuario pulsa sober "Calabaza" se saltará a `:salon_cesta_usar_calabaza`, o sea, prefijo, zona, accion, item.

# Text box

```
	tb "texto" ["titulo"] [wt] [cbc]
	tbwc "texto" ["titulo"]
```

Dibuja una caja de texto con `texto` y un título opcional `título`. Si se especifica `wt` se espera a que se haga click o se pulse ENTER. Si se especifica `cbc` se presenta el texto carácter a carácter. 

`tbwc` es una abreviatura de tb con `wt` y `cbc`, para escribir menos.

El cuadro de texto se configura con un porrón de parámetros:

```
	tb config x y w h c1 c2 tc1 tc2 f
```

* `x`, `y` las coordenadas (carácter) de la esquina superior izquierda. 
* `w`, `h` la anchura y la altura (en carácteres).
* `c1`, `c2` el color de primer plano y de fondo del texto.
* `tc1`, `tc2` el color de primer plano y de fondo del título opcional.
* `f` el color de la linea que hace de borde.

# cbc

```
	cbc n
```

Establece el delay del texto a "n" ticks. El valor por defecto es 4. Cuanto más, más lento. Hay 60 ticks en 1 segundo.

# resp

Permite mostrar un cuadro para elegir entre N respuestas, pensado para mantener conversaciones. Las respuestas se añaden al cuadro y al ejecutarse generará un número de 1 a N dependiendo de la respuesta elegida.

```
	resp config y c1 c2
```

Hay que tener en cuenta que si el número de opciones hace que el recuadro no quepa, éste se moverá verticalmente hasta que quepa. El recuadro siempre tomará el ancho de la pantalla con un márgen de 1 caracter por cada lado.

```
	resp reset
```

Eliminará todas las respuestas

```
	resp put "Este es el texto que se dice"
```

Añadirá una opción.

```
	resp run :label
```

generará saltos a `:label_N` con `N` = 1, 2... el número de la respuesta que el usuario pulse.

# extern

```
	extern a b [r]
```

Util si integras el intérprete en tu programa C. Esto llama a una función C `int extern (int a, int b)` que deberás implementar en `extern.h`. Si se especifica `r`, el valor numérico de la función se almacenará ahí.

# music

```
	music midi "spec" [loop]
	music mod "spec" [loop]
	music off
```

# sound

```
	sound play "spec" channel [loop]
	sound off channel
```

channel = 0..15. Probablemente los primeros estén ocupados por el midi o el mod.

# indicator

Muestra un caracter parpadeante en el "pause".

```
	indicator off|x y c1 c2 c
```

Desactivar (off) o activar en x, y, con colores c1, c2, caracter c (codigo de CP437). Por defecto c es 16, que representa un triángulo que apunta a la derecha.

# interface

```
	interface off|config "spec.gif" x y mask
```

Si se define una imagen, se cargará automáticamente en `x` `y` usando los pixeles de color mask como transparente de forma automática tras cada "bg", superponiéndola.

# Variables especiales

Empiezan por '@' y son estas:

* `@LAST_MENU_OPTION` devuelve el número de la última opción elegida en un menú, o -1 si no se eligió ninguna.
* `@MENU_OPTIONS_ADDED` devuelve el número de opciones que se añadió tras un `menu reset`.
* `@MENU_OPTIONS_REMOVED` devuelve el número de opciones que se eliminó tras un `menu_reset`.


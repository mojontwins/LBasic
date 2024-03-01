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

Borra la zona de texto de la pantalla (ver `viewport`);

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

Asigna a la flag `flag` el valor `valor`. `flag` debe resolver a un número entre 0 y 255. Ver la sección sobre flags.

LBasic soporta alias de forma transparente. Los alias se asignan a un número de flag y le dan una forma más amable de llamarlos. La asignación es automática:

```
	let alias valor
```

Creará `alias` y le asignará valor al flag asociado. Para referenciar este flag (del que no sabemos el número) usaremos la forma `%alias`. Por ejemplo:

```
	let perro 5
	tell %perro
```

**Esta es la únia forma de crear aliases de flags**. Para usar en un input antes deberá ser creado con let, aunque se le asigne un valor que luego jamás vaya a usarse. **Los aliases podrán tener hasta 32 caracteres**, si son más largos no se registrarán y el script no funcionará.

## input 

```
	input flag
```

Asigna a la flag `flag` el valor que introduzca el usuario. `flag` debe resolver a un número entre 0 y 255. Ver la sección sobre flags.

## tell

```
	tell flag
```

Escribe en pantalla el valor de la flag `flag` y deja el cursor donde está. `flag` debe resolver a un número entre 0 y 255. Ver la sección sobre flags.

## incc

```
	inc flag
```

Incrementa en 1 el valor de la flag `flag`. `flag` debe resolver a un número entre 0 y 255. Ver la sección sobre flags.

## dec

```
	dec flag
```

Decrementa en 1 el valor de la flag `flag`. `flag` debe resolver a un número entre 0 y 255. Ver la sección sobre flags.

## add

```
	add flag valor
```

Suma `valor` al valor de `flag` y lo almacena en `flag`. `flag` debe resolver a un número entre 0 y 255. Ver la sección sobre flags.

## sub

```
	sub flag valor
```

Resta `valor` del valor de `flag` y lo almacena en `flag`. `flag` debe resolver a un número entre 0 y 255. Ver la sección sobre flags.

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
	cut "pic.gif" x, y
```

Carga la imagen `pic.gif` en las coordenadas x, y.

Es equivalente a "CUT" de PRESENTS.EXE (Dr. Genius).

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
	menu add|pon "texto"
```

Añade al menú la opción con texto `texto`

```
	menu remove|quita "texto"
```

Quita del menú el la opción con texto `texto`

```
	menu config x y w c1 c2
```

Configura el menú para que aparezca en (x, y) con los colores (c1, c2), de anchura w

```
	menu run :label
```

Muestra el menú y deja al usuario elegir una opción, y luego:

- Si el usuario interrumpió el menú (ESC / 99 / etc) no hace nada! 
- Si el usuario eligió una opción, saltará a la etiqueta :label_opcion (*) (**) 
- ¡Ojo! si la etiqueta no existe, no hará nada.

(*) Se sustituirá los espacios por `_`, así una opción `COGER CAJA` saltará a `:label_coger_caja`
(**) en las etiquetas no importa mayúsculas o minúsculas. Mejor no poner acentos (por ahora)

Ejemplo con rayitas de colores:

![Ejemplo menus](https://github.com/mojontwins/LBasic/blob/master/docs/lbasic_menu.png?raw=true)

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
	talk config x y w c1 c2
```

Configura el texto para que aparezca en (x, y) con los colores (c1, c2), de anchura w

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
	zones def "texto" x1 y1 x2 y2 [actions]
```

Añade una zona con `texto` representando al rectángulo que va desde (x1, y1) hasta (x2, y2). Si se especifica `actions` esta zona desplegará el menú de acciones.

```
	zones run :label :label_ret
```

Se espera a que el jugador seleccione una zona y desplegará los menús encadenados que corresponda. La segunda etiqueta es adónde se volverá al ejecutar "ret".

```
	infobar off|y c1 c2
```

Desactiva o activa (y configura) una barra de información que se deplegará mientras el usuario se mueve por la pantalla con el ratón, mostrando el texto de la zona activa.

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

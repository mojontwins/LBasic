# LBasic Commands

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
- ww "what"
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
- mode text|gfx|gfx_sq|gfx_med|gfx_hi
- pic "pic.gif" seconds|kbd
- cut "cut.gif" x y
- sleep seconds
- menu

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
	ww "texto"
```

Escribe un texto con "word wrapping" y pasa a la linea siguiente. Este método ajusta las palabras del texto entre las columnas definidas en margins, de modo que si una palabra no cabe en una linea pasará a la siguiente. Es imporante emplear espacios tras la puntuación para que funcione de forma óptima.

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
	menu add|pon "item"
```

Añade al menú el item de texto "item"

```
	menu remove|quita "item"
```

Quita del menú el item de texto "item"

```
	menu config x y c1 c2
```

Configura el menú para que aparezca en (x, y) con los colores (c1, c2).

```
	menu run :label
```

Muestra el menú y deja al usuario elegir una opción, y luego:

- Si el usuario interrumpió el menú (ESC / 99 / etc) no hace nada! 
- Si el usuario eligió una opción, saltará a la etiqueta :label_opcion (*) (**) 
- ¡Ojo! si la etiqueta no existe, no hará nada.

(*) Se sustituirá los espacios por `_`, así una opción `COGER CAJA` saltará a `:label_coger_caja`
(**) en las etiquetas no importa mayúsculas o minúsculas. Mejor no poner acentos (por ahora)

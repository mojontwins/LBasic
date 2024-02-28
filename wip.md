# Backend

[X] Scroll bien hecho (teniendo en cuenta los margenes) -> necesito implementar blit de buffer a buffer (en modo gráfico) y complicar un poco la copia en modo texto.
[ ] Añadir clicks a PAD_THIS_FRAME y usarlo en los menús y zonas!

# Intérprete

[X] Legacy Lbasic 6
[ ] Legacy Lbasic 7
[/] Legacy Lbasic 8
[ ] GOSUB
[X] Autogo
[X] Inventario sencillo ~ parecido a menús, añadiendo Tokens a una lista
[X] Bulma "draw" support (LIN)
[ ] caracter parpadeante en pulsa una tecla: indicator off|on x y c1 c2 c
[ ] Bulma stuff
	- actions reset
	- actions put X
	- actions remove X

	- zones reset
	- zones def x1 x2 y1 y2 name [items|actions]
	- zones run :label

	:label_name / :label_name_item / :label_name_action

[ ] Controlar ESC en menús y zones para salir del juego <- reaprovechar TUI -- parametrizar cuadro de texto para que lo centre en cualquier resolución de pantalla!!!



# Editor

[X] Save / Load
[ ] wizards
	[ ] Choice 
	[ ] Choigo 
	[ ] Go 
	[X] Print 
	[ ] Color 
[X] RUN / RUN FROM / NEW
[X] Editor DRAW
[ ] Editor bulma draw (LIN)
[ ] AV PAG / RE PAG / HOME / END
[X] A veces se salva mal (creo que mezcla `\n` con `\r\n`), revisar. Cuando cargas->editas->grabas. En HEX EDITOR veo DOS \r... -> Grabar en binario
[X] Syntax no saca los textos entre comillas hasta que se cierran
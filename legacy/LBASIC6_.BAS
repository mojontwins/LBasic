DECLARE SUB openrecuadre (yyg, alto, ancho, sha1, sha2, c1, c2)
DECLARE SUB button (y, x, c1, c2, x$)
DECLARE SUB centra (y, T$)
DECLARE SUB manto (c1, c2)

DIM h$(3)
DIM y(4)
DIM help$(4)
DIM opt$(4)

CLS
SCREEN 0
ON ERROR GOTO errortrap

VIEW PRINT 1 TO 25
LOCATE 24, 1
COLOR 7, 1
PRINT "  Pulsa una tecla para comenzar ³ Introduce ' men£ ' siempre que quieras usarlo "
LOCATE 1, 1
COLOR 7, 1
PRINT "  Compilador Lokosoft LBasic versi¢n 6.03. (C)opyright 1994 by Loko Soft        "
COLOR 7, 0
LOCATE 3, 1
PRINT "*";

DO: LOOP WHILE INKEY$ = ""
GOSUB menu

editar:
	VIEW PRINT 2 TO 23
	LOCATE 3, 1

input1:
	err1 = 0
	LINE INPUT "* >Introduce el nombre del programa a hacer: "; n$

input2:
	LINE INPUT "* >Introduce el n£mero de bloque de programa (000-999)"; m$
	
	IF LEN(n$) > 8 OR n$ = "" THEN GOTO input1
	IF LEN(m$) <> 3 THEN GOTO input2
	
	FOR n = 1 TO LEN(m$)
		mm$ = MID$(m$, n, 1)
		IF ASC(mm$) < ASC("0") OR ASC(mm$) > ASC("9") THEN GOTO input2
	NEXT

	PRINT
	GOSUB convierteenmayusculas
	PRINT "  > " + n$ + "." + m$ + " <      ÀÄ ¨OK?"
	PRINT

input3:
	LINE INPUT "* >(S/N): "; s$
	IF ASC(s$) < ASC("a") THEN PRINT : PRINT "... Por favor, escribe en MINéSCULAS": GOTO input3
	IF s$ = "s" THEN GOTO okdone
	IF s$ = "n" THEN GOTO input1
	IF s$ <> "s" AND s$ <> "n" THEN PRINT "... Por favor, escribe 's' (SÖ) ¢ 'n' (NO).": GOTO input3

okdone:
	PRINT
	PRINT "* > OK. Empezar s a trabajar con el bloque N§ " + m$ + " de " + n$ + "."
	PRINT

input4:
	LINE INPUT "* > Introduce PATH (Pulsa ENTER para path actual):"; path$
	PRINT
	PRINT "PATH: ";
	IF path$ = "" THEN PRINT "Actual."
	IF path$ <> "" THEN PRINT path$: path$ = path$ + "\"
	PRINT

input5:
	INPUT "* > ¨Seguro? (s/n)"; s$
	IF ASC(s$) < ASC("a") THEN PRINT : PRINT "... Por favor, escribe en MINéSCULAS": GOTO input5
	IF s$ = "s" THEN GOTO stt
	IF s$ = "n" THEN GOTO input4
	IF s$ <> "s" AND s$ <> "n" THEN PRINT "... Por favor, escribe 's' (SÖ) ¢ 'n' (NO).": GOTO input5

stt:
	k$ = path$ + n$ + "." + m$

	OPEN k$ FOR OUTPUT AS #1
	IF err1 = 1 THEN GOTO input1
	PRINT
	PRINT
	PRINT "Ready..."
	PRINT

	textc1 = 7
	textc2 = 0

'' Parser principal
start:
	LINE INPUT "* "; p$
	IF p$ = "cursor" THEN GOTO cursorset
	'IF p$ = "cls" THEN GOTO clsset
	IF p$ = "beep" THEN GOTO beepset
	IF p$ = "draw" THEN GOTO drawset
	IF LEN(p$) >= 5 AND LEFT$(p$, 5) = "print" THEN GOTO printset
	IF LEN(p$) > 5 AND LEFT$(p$, 5) = "color" THEN GOTO colorset
	IF LEN(p$) > 4 AND LEFT$(p$, 4) = "play" THEN GOTO playset
	IF p$ = "choice" THEN GOTO choiceset
	IF LEN(p$) >= 5 AND LEFT$(p$, 5) = "pause" THEN GOTO pauseset
	IF LEN(p$) > 6 AND LEFT$(p$, 6) = "centra" THEN GOTO centraset
	IF p$ = "end" THEN GOTO endset
	IF p$ = "save" THEN GOTO saveset
	IF p$ = "quit" THEN GOTO quitset
	PRINT
	GOTO start

convierteenmayusculas:
	n2$ = ""
	FOR n = 1 TO LEN(n$)
	nn$ = MID$(n$, n, 1)
	LET nn$ = CHR$(ASC(nn$) - 32)
	n2$ = n2$ + nn$
	NEXT
	n$ = n2$
	n2$ = ""
	RETURN

errortrap:
	BEEP
	PRINT
	PRINT "* > ERROR!"
	PRINT
	PRINT "* > ERROR"; ERR; "en dispositivo."
	err1 = 1
	SELECT CASE ERR
		 CASE 76: PRINT "* > Error en el PATH (Path no existe)":  RESUME NEXT
		 CASE 64: PRINT "* > Nombre de archivo INCORRECTO":  RESUME NEXT
		 CASE 75: PRINT "* > Error de acceso en RUTA / ARCHIVO": RESUME NEXT
		 CASE 58: PRINT "* > Archivo ya existe, cree otro n£mero":  RESUME NEXT
		 CASE 71: PRINT "* > La unidad " + LEFT$(path$, 2) + " no est  lista": RESUME NEXT
		 CASE 62: GOTO ok01
	END SELECT
	RESUME NEXT

newpath:
	LINE INPUT "* > Nuevo PATH..."; path$
	RETURN

newnumber:
	LINE INPUT "* > Nuevo numero..."; m$
	RETURN

cursorset:
	PRINT #1, "@"
cur1:
	INPUT "Nueva X(1-80)?"; xx
	IF xx < 1 OR xx > 80 THEN PRINT "Valor fuera de rango...": GOTO cur1
cur2:
	INPUT "Nuevo Y(1-23)?"; yy
	IF yy < 1 OR yy > 23 THEN PRINT "Valor fuera de rango...": GOTO cur2
	PRINT #1, xx
	PRINT #1, yy
	GOTO start

beepset:
	PRINT #1, "#"
	GOTO start

drawset:
	d$ = ""

	'' TODO
	'' 1 per line d, chars, 0-7 fg, =!"ú$%&/ bg
	PRINT
	PRINT "Pulsa ESC cuando hayas terminado. BORRAR para borrar "
	PRINT "Ú q  Ä w  ¿ e   É r  Í t  » y     ° u  ± i  ² o  Û p      Ü l  ß ¤ "
	PRINT "³ a             º s        1-7 y 0 1er plano, SHIFT + 1-7 y 0 fondo"
	PRINT "À z       Ù x   È c       ¼ v     ÃV ´b Án Âm Åd Ìf ¹g Êh Ëj Îk    "

	c1 = 7     'primer plano
	c2 = 0     'fondo
	l0 = 0

	PRINT
	x = 1
	y = CSRLIN
	y0 = y

dra0:
	COLOR c1, c2
	LOCATE y, x: PRINT "ò"
	COLOR 7, 0

	f$ = INKEY$
	SELECT CASE (f$)
		CASE "q": p$ = "Ú": GOTO scriptum
		CASE "w": p$ = "Ä": GOTO scriptum
		CASE "e": p$ = "¿": GOTO scriptum
		CASE "r": p$ = "É": GOTO scriptum
		CASE "t": p$ = "Í": GOTO scriptum
		CASE "y": p$ = "»": GOTO scriptum
		CASE "u": p$ = "°": GOTO scriptum
		CASE "i": p$ = "±": GOTO scriptum
		CASE "o": p$ = "²": GOTO scriptum
		CASE "p": p$ = "Û": GOTO scriptum
		CASE "a": p$ = "³": GOTO scriptum
		CASE "s": p$ = "º": GOTO scriptum
		CASE "d": p$ = "Å": GOTO scriptum
		CASE "f": p$ = "Ì": GOTO scriptum
		CASE "g": p$ = "¹": GOTO scriptum
		CASE "h": p$ = "Ê": GOTO scriptum
		CASE "j": p$ = "Ë": GOTO scriptum
		CASE "k": p$ = "Î": GOTO scriptum
		CASE "l": p$ = "Ü": GOTO scriptum
		CASE "¤": p$ = "ß": GOTO scriptum
		CASE "z": p$ = "À": GOTO scriptum
		CASE "x": p$ = "Ù": GOTO scriptum
		CASE "c": p$ = "È": GOTO scriptum
		CASE "v": p$ = "¼": GOTO scriptum
		CASE "V": p$ = "Ã": GOTO scriptum
		CASE "b": p$ = "´": GOTO scriptum
		CASE "n": p$ = "Á": GOTO scriptum
		CASE "m": p$ = "Â": GOTO scriptum
		CASE "0": p$ = "0": c1 = 0: GOTO scrip2
		CASE "1": p$ = "1": c1 = 1: GOTO scrip2
		CASE "2": p$ = "2": c1 = 2: GOTO scrip2
		CASE "3": p$ = "3": c1 = 3: GOTO scrip2
		CASE "4": p$ = "4": c1 = 4: GOTO scrip2
		CASE "5": p$ = "5": c1 = 5: GOTO scrip2
		CASE "6": p$ = "6": c1 = 6: GOTO scrip2
		CASE "7": p$ = "7": c1 = 7: GOTO scrip2
		CASE "=": p$ = "=": c2 = 0: GOTO scrip2
		CASE "!": p$ = "!": c2 = 1: GOTO scrip2
		CASE CHR$(34): p$ = CHR$(34): c2 = 2: GOTO scrip2
		CASE "ú": p$ = "ú": c2 = 3: GOTO scrip2
		CASE "$": p$ = "$": c2 = 4: GOTO scrip2
		CASE "%": p$ = "%": c2 = 5: GOTO scrip2
		CASE "&": p$ = "&": c2 = 6: GOTO scrip2
		CASE "/": p$ = "/": c2 = 7: GOTO scrip2
		CASE " ": p$ = " ": GOTO scriptum
		CASE CHR$(27): GOTO final
		'    CASE CHR$(13): LOCATE y, x: PRINT " ": x = 1: y = y + 1: p$ = CHR$(13): GOTO scrip2
		CASE CHR$(8): GOTO borr
	END SELECT
	
	GOTO dra0

scriptum:
	COLOR c1, c2
	LOCATE y, x: PRINT p$
	x = x + 1
	IF x = 81 THEN x = 1: y = y + 1

scrip2:
	d$ = d$ + p$
	IF y = 14 THEN GOTO final
	GOTO dra0

borr:
	IF d$ = "" GOTO dra0

	br$ = ""
bohr:
	pq$ = RIGHT$(d$, 1)
	IF pq$ = "0" OR pq$ = "1" OR pq$ = "2" OR pq$ = "3" OR pq$ = "4" OR pq$ = "5" OR pq$ = "6" OR pq$ = "7" OR pq$ = "=" OR pq$ = "!" OR pq$ = CHR$(34) OR pq$ = "$" OR pq$ = "%" OR pq$ = "&" OR pq$ = "/" THEN br$ = br$ + pq$: d$ = LEFT$(d$, LEN(d$) - 1): GOTO bohr

	LOCATE y, x
	PRINT " "
	x = x - 1
	IF x = 0 THEN x = 80: y = y - 1
	d$ = LEFT$(d$, LEN(d$) - 1)
	d$ = d$ + br$
	GOTO dra0

final:
	PRINT #1, "d"
	FOR n = 1 TO LEN(d$)
		m$ = MID$(d$, n, 1)
		PRINT #1, m$
	NEXT n

	PRINT #1, "<END>"

	GOTO start

printset:
	IF p$ = "print" THEN PRINT #1, "p": GOTO start
	text$ = RIGHT$(p$, LEN(p$) - 6)
	PRINT #1, "p" + text$ + " "
	GOTO start

colorset:

	T$ = RIGHT$(p$, LEN(p$) - LEN("color "))

	b$ = ""
	FOR n = 1 TO LEN(T$)
		m$ = MID$(T$, n, 1)
		IF m$ = "," THEN k = n: GOTO col1
		b$ = b$ + m$
	NEXT
	textc1 = VAL(b$)
	GOTO colorsetset

col1:
	textc1 = VAL(b$)
	
	b$ = ""
	FOR n = k + 1 TO LEN(T$)
		m$ = MID$(T$, n, 1)
		b$ = b$ + m$
	NEXT
	textc2 = VAL(b$)

colorsetset:
	PRINT #1, "c" + LTRIM$(STR$(textc1)) + LTRIM$(STR$(textc2))

	GOTO start

playset:
	nota$ = RIGHT$(p$, LEN(p$) - 5)
	PRINT #1, "-" + nota$
	GOTO start

choiceset:
	p$ = ""
	PRINT : PRINT
	PRINT "** CHOICE **"
	p$ = p$ + "*"
repeaters:
	PRINT
	PRINT "¨Cu ntas opciones? (M ximo 3)"
	INPUT a
	IF a > 3 OR a < 1 THEN GOTO repeaters
repeaters2:
	PRINT
	PRINT "¨Cu l es la verdadera? (de 1 a " + STR$(a) + ")"
	INPUT b
	IF b < 1 OR b > a THEN GOTO repeaters2

	p$ = p$ + STR$(a) + STR$(b)
	FOR n = 1 TO a
		PRINT "> Introduce el texto que aparecer  una vez metido el n§ " + STR$(n) + ":"
		LINE INPUT T$
		p$ = p$ + T$ + ")"
	NEXT

	PRINT
	PRINT "** OK! **"
	PRINT
	PRINT #1, p$
	GOTO start

pauseset:
	PRINT #1, "¨"
	GOTO start

centraset:
	cntra$ = RIGHT$(p$, LEN(p$) - 7)
	PRINT #1, "‡" + cntra$
	GOTO start

endset:
	PRINT
	PRINT "¨ Seguro ?"
	PRINT
	PRINT "Esto har  que el programa dividido en bloques finalice y se cree un"
	PRINT "cargador para el programa. "
	PRINT
	LINE INPUT "(s ¢ n)  "; q$
	IF q$ = "s" THEN GOTO endd
	IF q$ = "n" THEN GOTO start
	GOTO start

endd:
	OPEN path$ + n$ + ".lbs" FOR OUTPUT AS #2
	PRINT #2, m$
	CLOSE #2
	PRINT
	PRINT "Salvando este £ltimo bloque (" + m$ + ")."

saveset:
	PRINT
	PRINT #1, "<FIN>"
	CLOSE #1
	PRINT
	PRINT "Pulsa una tecla para reiniciar el LBasic 6.0"
	DO: LOOP WHILE INKEY$ = "": RUN

quitset:
	PRINT
	PRINT "¨¨ Seguro que quieres regresar al MS DOS ??"
	LINE INPUT "* > (s ¢ n)"; q$
	IF q$ = "s" THEN SYSTEM
	GOTO start

selectthecolor:
	BEEP
	LOCATE 22, 1
	PRINT "... acabas de poner otro color."
	RETURN

menu:
	VIEW PRINT
	LOCATE 1, 1
	COLOR 7, 1
	PRINT "  Editar  ³  Corregir  ³  Ejecutar un programa  ³      (C) Versi¢n 6.03, 1994 "
	choice = 1
	y(1) = 1
	y(2) = 12
	y(3) = 25
	y(4) = 50
	opt$(1) = "  Editar  "
	opt$(2) = "  Corregir  "
	opt$(3) = "  Ejecutar un programa  "
	opt$(4) = "  Imprimir programa  "
	help$(1) = "Para hacer un programa"
	help$(2) = "Para hacer correcciones en un programa"
	help$(3) = "Para jugar a un programa que tu hayas hecho"
	help$(4) = "Para imprimir un programa que tu hayas hecho"

	FOR n = 1 TO 4
		help$(n) = help$(n) + SPACE$(80 - LEN(help$(n)))
	NEXT

kodansha:
	GOSUB imprimir
	k$ = INKEY$
	SELECT CASE k$
		CASE CHR$(0) + "K": GOSUB left
		CASE CHR$(0) + "M": GOSUB right
		CASE CHR$(13): GOSUB intro
	END SELECT
	GOTO kodansha

imprimir:
	LOCATE 1, y(choice)
	COLOR 9, 7
	PRINT opt$(choice)
	VIEW PRINT 1 TO 25
	LOCATE 24, 1
	COLOR 7, 1
	PRINT help$(choice)
	RETURN

borrar:
	LOCATE 1, y(choice)
	COLOR 7, 1
	PRINT opt$(choice)
	RETURN

left:
	IF choice > 1 THEN GOSUB borrar: choice = choice - 1
	RETURN

right:
	IF choice < 3 THEN GOSUB borrar: choice = choice + 1
	RETURN

intro:
	COLOR 7, 0
	IF choice = 1 THEN GOTO editar
	IF choice = 2 THEN GOTO corregir
	IF choice = 3 THEN GOTO ejecutar
	IF choice = 4 THEN GOTO imprimirr
	RETURN

corregir:
	VIEW PRINT 2 TO 23
	CLS
	PRINT
	PRINT "  Corregir  "

corregir2:
	PRINT
	PRINT "Introduce el path donde est  tu programa."
	PRINT "(Enter si est  aqu¡)"
	PRINT
	err1 = 0
	LINE INPUT "* > "; pp$
	IF pp$ <> "" AND RIGHT$(pp$, 1) <> "\" THEN pp$ = pp$ + "\"
	FILES pp$
	IF err1 = 1 THEN GOTO corregir2
	PRINT
	PRINT "¨Est  aqu¡ tu programa?"
	PRINT
	LINE INPUT "* > (s)¡ o (n)o..."; w$
	IF w$ = "n" THEN GOTO corregir2
	CHDIR LEFT$(pp$, LEN(pp$) - 1)

corregir3:
	PRINT
	PRINT "Introduce Nombre y Extensi¢n del programa a corregir..."
	PRINT
	LINE INPUT "* > Nombre y extensi¢n:"; q$
	err1 = 0
	OPEN pp$ + q$ FOR INPUT AS #1
	IF err1 = 1 THEN GOTO corregir3
	OPEN pp$ + "renombre" FOR OUTPUT AS #2
	PRINT
	CLS
	lin = 1

lolo:
	CLS
	INPUT #1, n$
	PRINT
	COLOR 15
	PRINT "L¡nea"; lin; "del programa "; q$
	COLOR 7
	PRINT
	PRINT n$
	IF n$ = "<FIN>" THEN GOTO neww
	PRINT
	LINE INPUT "¨Corregir? (s)¡ ¢ (n)o."; a$
	IF a$ = "s" THEN GOTO corregir4 ELSE a$ = n$: GOTO siguiente

corregir4:
	PRINT
	PRINT "Nueva l¡nea:"
	PRINT
	LINE INPUT a$

siguiente:
	PRINT #2, a$
	lin = lin + 1
	GOTO lolo

neww:
	CLOSE #1
	PRINT #2, "<FIN>"
	CLOSE #2
	KILL pp$ + q$
	NAME pp$ + "renombre" AS pp$ + q$
	PRINT
	PRINT "pulsa una tecla"
	DO: LOOP WHILE INKEY$ = ""
	RUN
	END

ejecutar:
	CLS
	PRINT
	PRINT "                    EJECUTAR UN PROGRAMA"
	PRINT
	PRINT "Introduce el path, el s¡mbolo '\' y el nombre sin indicar la extensi¢n"
	PRINT "del programa a ejecutar. El ordenador buscar  ese nombre con la extensi¢n"
	PRINT "'.lbs', que es el controlador. Para que ese controlador haya sido escri-"
	PRINT "to, tienes que haber puesto en el editor la opci¢n 'end', la cual finaliza"
	PRINT "el programa y crea un archivo con el n£mero de bloques..."
	PRINT
	PRINT "¨Hiciste eso con el programa que vas a cargar?"
	LINE INPUT "(s)¡ ¢ (n)o "; q$
	IF q$ = "n" THEN PRINT : PRINT "Pulsa una tecla para reiniciar LBasic 6.0": DO: LOOP WHILE INKEY$ = "": RUN
	PRINT
	PRINT "Bien, pues introduce el path, el s¡mbolo '\' y el nombre sin extensi¢n."
	PRINT
	LINE INPUT nombre$
	err1 = 0

	OPEN nombre$ + ".lbs" FOR INPUT AS #1
	IF err1 = 1 THEN DO: LOOP WHILE INKEY$ = "": RUN
	err1 = 0
	textc1 = 7
	textc2 = 0
	PRINT
	INPUT #1, m$
	CLOSE #1

	'obtengo el n£mero de bloques
	numerodebloques = VAL(m$)

	resetblok% = -0

	'ahora empiezo a ejecutar los bloques uno por uno
	FOR currentblock = 0 TO numerodebloques
	
		m1$ = STR$(currentblock)                  ' Para obtener el n£mero del bloque
		m2$ = RIGHT$(m1$, LEN(m1$) - 1)           ' actual realizo estas operaciones
		m3$ = STRING$(3 - LEN(m2$), "0") + m2$    ' con cadenas y obtengo 'm3$'.

		'ahora obtengo el nombre del bloque actual
		bloqueactual$ = nombre$ + "." + m3$       ' por ejemplo: 'NADIA.e00'


bloqueactual:
		
		'abro el archivo #1 para entrada de datos
		OPEN bloqueactual$ FOR INPUT AS #1
		CLS
		IF resetblok% THEN
			centra 12, "Repitiendo bloque " + m2$
			resetblok% = 0
		ELSE
			centra 12, "Empezando bloque " + m2$
		END IF

		DO: LOOP WHILE INKEY$ = ""
	   
		CLS
		lin = 1

		' empieza el programa que lee los datos del archivo

leer:
		INPUT #1, a$       ' La variable 'a$' contendr  la l¡nea en curso.

		IF a$ = "<FIN>" THEN GOTO ok01: ' se acab¢ el bloque...
	
		g$ = MID$(a$, 1, 1)' La variable 'g$' contendr  la 1¦ letra (Indicador)
	
		SELECT CASE g$     ' ahora comprobaremos qu‚ letra contiene 'g$'.
			CASE "=": CLS
			CASE "#": BEEP
			CASE "¨": DO: LOOP WHILE INKEY$ = ""
			CASE "?": SLEEP 1
			CASE "c": GOSUB runcolor
			CASE "*": GOSUB runchoice
			CASE "p": GOSUB runprint
			CASE "‡": GOSUB runcentra
			CASE "@": INPUT #1, x: INPUT #1, y: LOCATE y, x
			CASE "d": GOSUB rundraw
			CASE "-": GOSUB runplay
		END SELECT

		IF resetblok% = -1 THEN GOTO ok02

		lin = lin + 1
		GOTO leer

ok01:
		PRINT "Pulsa una tecla"
		DO: LOOP WHILE INKEY$ = ""
		CLOSE #1

	NEXT currentblock

	PRINT "0, OK, 0:1"
	DO: LOOP WHILE INKEY$ = "": RUN

ok02:
	CLOSE #1
	GOTO bloqueactual

runcolor:
	d$ = MID$(a$, 2, 2)
	e$ = MID$(a$, 4, 2)
	d = VAL(d$)
	e = VAL(e$)
	COLOR d, e
	textc1 = d
	textc2 = e
	RETURN

runchoice:
	nu$ = MID$(a$, 3, 1): num = VAL(nu$)
	ve$ = MID$(a$, 5, 1): ver = VAL(ve$)
	x = 5

	FOR n = 1 TO num
		h$(n) = ""
repeatdone:
		x = x + 1
		IF MID$(a$, x, 1) = ")" THEN GOTO nextdone
		h$(n) = h$(n) + MID$(a$, x, 1)
		GOTO repeatdone
nextdone:
	NEXT
	PRINT

	INPUT r
	
	IF r = ver THEN GOTO pass ELSE PRINT h$(r): PRINT "... ­GAME OVER!": PRINT "Deber¡as haber puesto"; ver: DO: LOOP WHILE INKEY$ = "": resetblok% = -1: RETURN

pass:
	PRINT h$(r)
	PRINT
	RETURN

runprint:
	h$ = RIGHT$(a$, LEN(a$) - 1)
	PRINT h$
	RETURN

runcentra:
centra:
	h$ = RIGHT$(a$, LEN(a$) - 1)
	l = 40 - LEN(h$) / 2
	PRINT TAB(l); h$
	RETURN

rundraw:
	' variables de dibujo
	x = 1  'variables de posici¢n
	y = 1  'en la pantalla.
	cc1 = textc1' color actual. (1er plano)
	cc2 = textc2' idem antes. (fondo)
	' ahora comienza el bloque lee-datos:
bloqueleer:
	INPUT #1, a$ 'lee el dato y lo pone en 'a$'
	IF a$ = "<END>" THEN COLOR textc1, textc2: RETURN 'pone el color verdadero y ­adi¢s!
	LOCATE y, x
	IF a$ = "1" OR a$ = "2" OR a$ = "3" OR a$ = "4" OR a$ = "5" OR a$ = "6" OR a$ = "7" OR a$ = "0" OR a$ = "!" OR a$ = CHR$(34) OR a$ = "ú" OR a$ = "$" OR a$ = "%" OR a$ = "&" OR a$ = "/" OR a$ = "=" THEN GOSUB selectcolors: GOTO nexff
	PRINT a$
	x = x + 1
	IF x >= 81 THEN x = 1: y = y + 1
nexff:
	GOTO bloqueleer

selectcolors:
	IF a$ = "1" OR a$ = "2" OR a$ = "3" OR a$ = "4" OR a$ = "5" OR a$ = "6" OR a$ = "7" OR a$ = "0" THEN cc1 = VAL(a$) + 8
	IF a$ = "!" THEN cc2 = 1
	IF a$ = CHR$(34) THEN cc2 = 2
	IF a$ = "ú" THEN cc2 = 3
	IF a$ = "$" THEN cc2 = 4
	IF a$ = "%" THEN cc2 = 5
	IF a$ = "&" THEN cc2 = 6
	IF a$ = "/" THEN cc2 = 7
	IF a$ = "=" THEN cc2 = 0
	COLOR cc1, cc2
	RETURN

runplay:
	err1 = 0
	nota$ = RIGHT$(a$, LEN(a$) - 1)
	PLAY nota$
	IF err1 = 1 THEN PRINT "­ Error en la l¡nea " + STR$(lin) + " en el bloque " + m2$ + " en notas !"
	err1 = 0
	RETURN

imprimirr:

SUB button (y, x, c1, c2, x$)
	LOCATE y, x
	PRINT "  Intro  "
	LOCATE y + 1, x + 2
	PRINT STRING$(9, "Û")
	
	DO
	LOOP WHILE INKEY$ <> CHR$(13)
	
	LOCATE y, x
	PRINT STRING$(9, x$)
	LOCATE y + 1, x + 2
	PRINT "  Intro  "
	
	T! = TIMER: DO: LOOP WHILE TIMER < T! + .5
END SUB

SUB centra (y, T$)
	LOCATE y, 40 - LEN(T$) / 2
	PRINT T$
END SUB

SUB manto (c1, c2)
	COLOR c1, c2
	VIEW PRINT 1 TO 25
	FOR n = 0 TO 25
		PRINT STRING$(80, "°")
	NEXT
	VIEW PRINT
END SUB

SUB openrecuadre (yyg, alto, ancho, sha1, sha2, c1, c2)
	alto = alto / 2
	ancho = ancho / 2
	FOR n = 2 TO alto
		COLOR c1, c2
		LOCATE yyg - n, 38
		PRINT "ÚÄÄ¿"
		LOCATE yyg + n, 38
		PRINT "ÀÄÄÙ"
		LOCATE yyg - (n - 1), 38
		PRINT "³  ³"
		LOCATE yyg + n - 1, 38
		PRINT "³  ³"
		
		T! = TIMER: DO: LOOP WHILE TIMER < T! + .05
	NEXT

	LOCATE yyg, 40: PRINT " "
	
	FOR n = 2 TO ancho
		FOR y = yyg - alto TO yyg + alto
			COLOR c1, c2
			IF y = yyg - alto OR y = yyg + alto THEN LOCATE y, 40 - (n - 1): PRINT "Ä": LOCATE y, 40 + n - 1: PRINT "Ä"
			IF y = yyg - alto THEN LOCATE y, 40 - n: PRINT "Ú": LOCATE y, 40 + n: PRINT "¿"
			IF y = yyg + alto THEN LOCATE y, 40 - n: PRINT "À": LOCATE y, 40 + n: PRINT "Ù"
			IF y <> yyg - alto AND y <> yyg + alto THEN LOCATE y, 40 - n: PRINT "³ ": LOCATE y, 40 + n - 1: PRINT " ³"
			COLOR sha1, sha2
			LOCATE y + 1, 40 + n + 1: PRINT "²²"
			IF y = yyg + alto THEN LOCATE y + 1, 40 - n + 2: PRINT "²²²"
		NEXT

		T! = TIMER: DO: LOOP WHILE TIMER < T! + .05
	NEXT
	COLOR c1, c2
END SUB


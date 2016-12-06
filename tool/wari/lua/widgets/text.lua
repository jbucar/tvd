-- El objetivo de este widget es dado un rect, renderice un texto con una fuente dada, de modo que agregue un
-- salto de linea al texto cada vez que sea necesario para que aproveche no solo el ancho, sino también el alto del
-- rectángulo.
--

function setFittingTextToLabel( label, w, h, txt)

	label.surface:clear()
	_, _, ascent = measureText(txt, label.fontSize)
	if ( (label.w ~= w) or (label.h ~= h) ) then
		label.w = w
		label.h = h
		label.surface:resize(label.w, label.h)
	end

	label.surface:drawText(0, 0, w-1, h-1, txt, "byWord", 2)
end

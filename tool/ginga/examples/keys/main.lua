canvas:attrFont('Tiresias',26,'bold')
function handler(evt)
    if evt.class == 'key' then
        canvas:attrColor('black')
        canvas:clear()
        canvas:attrColor('white')
        canvas:drawText(100,100,'evt.class = '..evt.class)
        canvas:drawText(100,150,'evt.type = '..evt.type)
        canvas:drawText(100,200,'evt.key = '..evt.key)
        canvas:flush()
    end
end
event.register(handler)
# trueTools: Text to HTML

This stack simply converted given text to HTML,
i.e. it replaced some characters by their HTML escapes
(like "&amp;" for "&") plus detected bold and italic text.

(The actual replacements were configurable via a card button,
that is why they are not visible in the script).

## Card Button "Convert" Script

```
on mouseUp
  lock screen
  put rect of btn "frame" into r
  set rect of btn "progress" to item 1 of r,item 2 of r, ¬
    item 1 of r, item 4 of r
  put item 3 of r - item 1 of r into w
  put empty into cd fld "progress"
  show btn "cover"
  show btn "progress"
  show btn "frame"
  show btn "Stop"
  show cd fld "progress"
  unlock screen
  --
  put cd fld "index" into index
  put cd fld "in" into a
  put empty into b
  put false into isBold
  put false into isItalic
  put the number of chars of a into n
  put the number of lines of a into nn
  --
  put 0 into ii
  repeat with i=1 to n
    if i mod 16 = 0 then set cursor to busy
    if the mouse is down and the mouseLoc is within ¬
      the rect of btn "Stop" then
      send restore to this card
      exit mouseUp --∆
    end if
    --
    put textStyle of char i of cd fld "in" into s
    --
    if "Bold" is in s and not isBold then
      put "<b>" after b
      put true into isBold
    else if "Bold" is not in s and isBold then
      put "</b>" after b
      put false into isBold
    end if
    --
    if "Italic" is in s and not isItalic then
      put "<i>" after b
      put true into isItalic
    else if "Italic" is not in s and isItalic then
      put "</i>" after b
      put false into isItalic
    end if
    --
    put char i of a into c
    if c is return then
      lock screen
      add 1 to ii
      put ii && "of" && nn into cd fld "progress"
      put rect of btn "progress" into r
      put item 1 of r + trunc(w*i/n) into item 3 of r
      set rect of btn "progress" to r
      unlock screen
      put "<br>" & return after b
    else if c ≥ " " then
      put line (charToNum(c)-31) of index into x
      put char 5 to (number of chars of x) of x after b
    end if
  end repeat
  --
  put cd fld "header" & return & b & return & cd fld "footer" ¬
    into cd fld "out"
  send restore to this card
end mouseUp
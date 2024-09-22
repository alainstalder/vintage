# trueTools: vue

This is my favorite stack in the "trueTools" collection.
It allowed to view images full screen without distractions,
with possibility to zoom in and out, and to move around.

See also its [readme](../trueTools-vue.pdf).

# Stack Script

This is one of my favorite short source codes ever.

```
on vue f, kind, dither, scale, rs
  --
  if kind is empty then put "file" into kind
  put dither≠false into dither
  if rs is empty then put the screenRect into rs
  --
  put the windows into a
  picture f, kind, rect, false
  if the result is not empty then return the result  --∆
  put the windows into b
  --
  put the number of lines of b into n
  if the number of lines of a + 1 ≠ n then
    return "Could not open window."  --∆
  end if
  --
  repeat with i=1 to n
    if line i of b ≠ line i of a then exit repeat
  end repeat
  put id of window i into wd
  --
  set rect of window id wd to 0,0,1,1
  set scroll of window id wd to 30000,30000
  put width of window id wd + item 1 of scroll of window id wd into wp
  put height of window id wd + item 2 of scroll of window id wd into hp
  --
  put "trou noir" into black
  picture black, resource, rect, false
  --
  if the result is not empty then
    put the result into r
    close window id wd
    return r  --∆
  end if
  --
  set the scale of window black to 5
  set globalRect of window black to rs
  --
  put item 1 of rs into ls
  put item 2 of rs into ts
  put item 3 of rs - ls into ws
  put item 4 of rs - ts into hs
  --
  put rect of card window into rw
  put item 1 of rw into lw
  put item 2 of rw into tw
  --
  if scale is empty then
    put 0 into scale
    if wp*2≤ws and hp*2≤hs then
      repeat while scale ≤ 5 and (wp*2≤ws and hp*2≤hs)
        add 1 to scale
        put wp*2 into wp
        put hp*2 into hp
      end repeat
    else
      repeat while scale ≥ -5 and (wp>ws or hp>hs)
        subtract 1 from scale
        put wp/2 into wp
        put hp/2 into hp
      end repeat
    end if
  else if scale>0 then
    repeat with i=scale down to 1
      put wp*2 into wp
      put hp*2 into hp
    end repeat
  else if scale<0 then
    repeat with i=scale to -1
      put wp/2 into wp
      put hp/2 into hp
    end repeat
  end if
  --
  put trunc((ws-wp)/2)+ls into lp
  put trunc((hs-hp)/2)+ts into tp
  --
  put lp & "," & tp & "," ¬
    & lp+trunc(wp) & "," & tp+trunc(hp) into r
  set the scale of window  id wd to scale
  set globalRect of window id wd to r
  set dithering of window id wd to dither
  --
  put menubarVisible() into hideMenu
  if hideMenu then hide menubar
  show window black
  show window id wd
  --
  put the mouseLoc into m
  put ticks()-31 into t0             -- hide cursor
  --
  repeat while true
    repeat while the mouse is up
      if the shiftKey is down then doFKEY(3) -- screen shot
      if the optionKey is down then
        put the mouseLoc into m
        put ticks() into t0
        if scale>-5 then set cursor to "moins vu"
        else set cursor to "vu moins blanc"
      else if the commandKey is down then
        put the mouseLoc into m
        put ticks() into t0
        if scale<5 then set cursor to "plus vu"
        else set cursor to "vu plus blanc"
      else
        put the mouseLoc into m1
        if m=m1 and ticks()-t0>30 then set cursor to "none"
        else
          set cursor to "point de vue"
          if m≠m1 then
            put the mouseLoc into m
            put ticks() into t0
          end if
        end if
      end if
    end repeat
    --
    if the optionKey is down then
      if scale>-5 then                       -- zoom out
        put the mouseLoc into m
        --
        subtract 1 from scale
        put wp/2 into wp
        put hp/2 into hp
        --
        if wp>ws then
          put item 1 of m + lw into lm
          add trunc((lm-lp)/2) to lp
          if lp>ls then put ls into lp
          else if lp+wp < ls+ws then
            put ls+ws-wp into lp
          end if
          put trunc(lp) into lp
        else
          put trunc((ws-wp)/2)+ls into lp
        end if
        --
        if hp>hs then
          put item 2 of m + tw into tm
          add trunc((tm-tp)/2) to tp
          if tp>ts then put ts into tp
          else if tp+hp < ts+hs then
            put ts+hs-hp into tp
          end if
          put trunc(tp) into tp
        else
          put trunc((hs-hp)/2)+ts into tp
        end if
        --
        put lp & "," & tp & "," ¬
          & lp+trunc(wp) & "," & tp+trunc(hp) into r
        set the scale of window id wd to scale
        set globalRect of window id wd to r
      end if
      --
      wait until the mouse is up
      --
    else if the commandKey is down then
      if scale<5 then                       -- zoom in
        put the mouseLoc into m
        --
        add 1 to scale
        put wp*2 into wp
        put hp*2 into hp
        --
        if wp>ws then
          put item 1 of m + lw into lm
          subtract lm-lp from lp
          if lp>ls then put ls into lp
          else if lp+wp < ls+ws then
            put ls+ws-wp into lp
          end if
          put trunc(lp) into lp
        else
          put trunc((ws-wp)/2)+ls into lp
        end if
        --
        if hp>hs then
          put item 2 of m + tw into tm
          subtract tm-tp from tp
          if tp>ts then put ts into tp
          else if tp+hp < ts+hs then
          put ts+hs-hp into tp
        end if
        put trunc(tp) into tp
      else
        put trunc((hs-hp)/2)+ts into tp
      end if
      --
      put lp & "," & tp & "," ¬
        & lp+trunc(wp) & "," & tp+trunc(hp) into r
      set the scale of window id wd to scale
      set globalRect of window id wd to r
    end if
    --
    wait until the mouse is up
    --
    else
      wait 12 ticks
      if the mouse is up then exit repeat  --∆
      else
        if wp>ws or hp>hs then                   -- move
          set cursor to "patte de mimi"
          --
          put the mouseH into lm
          put the mouseV into tm
          put lm into lm1
          put tm into tm1
          --
          put lp into lp1
          put tp into tp1
          --
          repeat while the mouse is down
            put the mouseH into lm2
            put the mouseV into tm2
            --
            if lm1≠lm2 or tm1≠tm2 then
              put lp-lm+lm2 into lp2
              put tp-tm+tm2 into tp2
              --
              if wp<ws then
                add lm2-lm1 to lm
                put lp-lm+lm2 into lp2
              else if lp2>ls then
                add lp2-ls to lm
                put ls into lp2
              else if lp2+wp < ls+ws then
                subtract ls+ws-(lp2+wp) from lm
                put ls+ws-wp into lp2
              end if
              --
              if hp<hs then
                add tm2-tm1 to tm
                put tp-tm+tm2 into tp2
              else if tp2>ts then
                add tp2-ts to tm
                put ts into tp2
              else if tp2+hp < ts+hs then
                subtract ts+hs-(tp2+hp) from tm
                put ts+hs-hp into tp2
              end if
              --
              if lp1≠lp2 or tp1≠tp2 then
                put lp2 into lp1
                put tp2 into tp1
                --
                put lp1 & "," & tp1 & "," ¬
                  & lp1+trunc(wp) & "," & tp1+trunc(hp) into r
                set globalRect of window id wd to r
              end if
            end if
            --
            put lm2 into lm1
            put tm2 into tm1
            --
          end repeat
          put lp1 into lp
          put tp1 into tp
        else
          wait until the mouse is up
        end if
      end if
    end if
  end repeat
  --
  close window id wd
  close window black
  --
  if hideMenu then
    show menubar
    if not menubarVisible() then show menubar -- is necessairy
  end if
end vue
```

## Card Button "Barbarella" Script

The button with an iris icon that was used to start viewing
on the delivered stack, while, of course, the same script
could have been used on other stacks, too.

```
on mouseDown
  vue cd fld "name"
  if the result is not empty and the result≠"Cancel" then
    answer the result
  end if
end mouseDown
```

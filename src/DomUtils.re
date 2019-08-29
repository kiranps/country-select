[@bs.scope "document"] [@bs.val]
external addKeybordEventListener:
  (string, ReactEvent.Keyboard.t => unit) => unit =
  "addEventListener";

[@bs.scope "document"] [@bs.val]
external removeKeybordEventListener:
  (string, ReactEvent.Keyboard.t => unit) => unit =
  "removeEventListener";

[@bs.send.pipe: Dom.element]
external getElementsByClassName: string => Dom.htmlCollection = "";

[@bs.get] external nextElementSibling: Dom.element => Dom.element = "";

[@bs.get] external focus: Dom.element => unit = "";

[@bs.send.pipe: Dom.element]
external scrollIntoViewIfNeeded: bool => unit = "";

[@bs.send.pipe: Dom.htmlCollection] [@bs.return nullable]
external item: int => option(Dom.element) = "item";
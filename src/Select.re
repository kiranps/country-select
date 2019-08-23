Utils.require("./style.css");

type t = {
  value: string,
  label: string,
};

[@react.component]
let make = (~values: list(t)=[], ~onChange: option(t) => unit=?) => {
  let (value, setValue) = React.useState(_ => None);
  let (isOpen, setOpen) = React.useState(_ => false);

  let toggleOpen = _ => {
    setOpen(isOpen => !isOpen);
  };

  let handleClose = e => {
    ReactEvent.Mouse.stopPropagation(e);
    setValue(_ => None);
    onChange(None);
  };

  let handleSelect = value => {
    setValue(_ => Some(value));
    onChange(Some(value));
  };

  let divRef = Hooks.useClickOutside(_ => setOpen(_ => false));

  <div
    className="select" ref={ReactDOMRe.Ref.domRef(divRef)} onClick=toggleOpen>
    <div>
      <div className="selected-value">
        <div className="label">
          {(
             switch (value) {
             | Some(option) => option.label
             | None => "select"
             }
           )
           |> React.string}
        </div>
        {switch (value) {
         | Some(_) =>
           <div className="close-icon" onClick=handleClose>
             {React.string("x")}
           </div>
         | None => React.null
         }}
      </div>
      {isOpen
         ? <div className="menu">
             {values
              |> List.mapi((i, x) =>
                   <div
                     key={string_of_int(i)}
                     className="menu-item"
                     onClick={_ => handleSelect(x)}>
                     {React.string(x.label)}
                   </div>
                 )
              |> Array.of_list
              |> React.array}
           </div>
         : React.null}
    </div>
  </div>;
};
Utils.require("./style.css");
Utils.require("flag-icon-css/css/flag-icon.css");

type t = {
  value: string,
  label: string,
};

[@react.component]
let make = (~values: list(t)=[], ~onChange: option(t) => unit=?) => {
  let (value, setValue) = React.useState(_ => None);
  let (isOpen, setOpen) = React.useState(_ => false);
  let (searchValue, setSearchValue) = React.useState(_ => "");

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
    setOpen(_ => false);
    onChange(Some(value));
  };

  let handleSearch = e => {
    let value = ReactEvent.Form.target(e)##value |> String.lowercase;
    setSearchValue(_ => value);
    ();
  };

  Js.log(searchValue);

  let filteredValues =
    searchValue === ""
      ? values
      : List.filter(
          x => Js.String.includes(searchValue, String.lowercase(x.label)),
          values,
        );

  let divRef = Hooks.useClickOutside(_ => setOpen(_ => false));

  <div className="select" ref={ReactDOMRe.Ref.domRef(divRef)}>
    <div>
      <div className="selected-value" onClick=toggleOpen>
        <div className="label">
          {(
             switch (value) {
             | Some(option) => option.label
             | None => "select country"
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
         ? <div className="dropdown">
             <div className="search">
               <svg viewBox="0 0 24 24">
                 <path
                   fill="#000000"
                   d="M9.5,3A6.5,6.5 0 0,1 16,9.5C16,11.11 15.41,12.59 14.44,13.73L14.71,14H15.5L20.5,19L19,20.5L14,15.5V14.71L13.73,14.44C12.59,15.41 11.11,16 9.5,16A6.5,6.5 0 0,1 3,9.5A6.5,6.5 0 0,1 9.5,3M9.5,5C7,5 5,7 5,9.5C5,12 7,14 9.5,14C12,14 14,12 14,9.5C14,7 12,5 9.5,5Z"
                 />
               </svg>
               <input onChange=handleSearch />
             </div>
             <div className="menu">
               {filteredValues
                |> List.mapi((i, x) =>
                     <div
                       key={string_of_int(i)}
                       className="menu-item"
                       onClick={_ => handleSelect(x)}>
                       <span className={"flag-icon flag-icon-" ++ x.value} />
                       <span> {React.string(x.label)} </span>
                     </div>
                   )
                |> Array.of_list
                |> React.array}
             </div>
           </div>
         : React.null}
    </div>
  </div>;
};
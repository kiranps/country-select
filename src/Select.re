open Hooks;
Utils.require("./style.css");
Utils.require("flag-icon-css/css/flag-icon.css");

type t = {
  value: string,
  label: string,
};

type state = {
  isOpen: bool,
  value: option(t),
  activeIndex: int,
  filteredValues: list(t),
};

type action =
  | Next
  | Prev
  | Hide
  | ToggleOpen
  | FilterValues
  | Clear
  | Search(list(t), string)
  | Select(t);

let next = (x, len) => x == len - 1 ? 0 : x + 1;
let prev = (x, len) => x == 0 ? len - 1 : x - 1;

let filterValues = (values, searchText) =>
  searchText === ""
    ? values
    : List.filter(
        x => Js.String.includes(searchText, String.lowercase(x.label)),
        values,
      );

let reducer = (state, action) =>
  switch (action) {
  | Next => {
      ...state,
      activeIndex:
        next(state.activeIndex, List.length(state.filteredValues)),
    }
  | Prev => {
      ...state,
      activeIndex:
        prev(state.activeIndex, List.length(state.filteredValues)),
    }
  | ToggleOpen => {...state, isOpen: !state.isOpen}
  | Hide => {...state, isOpen: false}
  | Clear => {...state, value: None}
  | Select(choice) => {...state, value: Some(choice), isOpen: false}
  | Search(options, text) => {
      ...state,
      filteredValues: filterValues(options, text),
    }
  };

let initialState = {
  isOpen: false,
  value: None,
  activeIndex: (-1),
  filteredValues: [],
};

[@react.component]
let make = (~values: list(t)=[], ~onChange: option(t) => unit=?) => {
  let (state, dispatch) = React.useReducer(reducer, initialState);

  React.useEffect1(
    () => {
      dispatch(Search(values, ""));
      Some(() => ());
    },
    [|values|],
  );

  let handleCallback = e =>
    switch (ReactEvent.Keyboard.key(e)) {
    | "ArrowUp" => dispatch(Prev)
    | "ArrowDown" => dispatch(Next)
    | _ => Js.log("")
    };

  React.useEffect0(() => {
    DomUtils.addKeybordEventListener("keydown", handleCallback);
    Some(
      () => DomUtils.removeKeybordEventListener("keydown", handleCallback),
    );
  });

  let handleClear = e => {
    ReactEvent.Mouse.stopPropagation(e);
    dispatch(Clear);
    onChange(None);
  };

  let handleSelect = value => {
    dispatch(Select(value));
    onChange(Some(value));
  };

  let handleSearch = e => {
    let value = ReactEvent.Form.target(e)##value |> String.lowercase;
    dispatch(Search(values, value));
  };

  let divRef = useClickOutside(_ => dispatch(Hide));

  <div className="select" ref={ReactDOMRe.Ref.domRef(divRef)}>
    <div>
      <div className="selected-value" onClick={_ => dispatch(ToggleOpen)}>
        <div className="label">
          {(
             switch (state.value) {
             | Some(option) => option.label
             | None => "select country"
             }
           )
           |> React.string}
        </div>
        {switch (state.value) {
         | Some(_) =>
           <div className="close-icon" onClick=handleClear>
             {React.string("x")}
           </div>
         | None => React.null
         }}
      </div>
      {state.isOpen
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
               {state.filteredValues
                |> List.mapi((i, x) =>
                     <div
                       key={string_of_int(i)}
                       className={
                         "menu-item"
                         ++ (i === state.activeIndex ? "--active" : "")
                       }
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
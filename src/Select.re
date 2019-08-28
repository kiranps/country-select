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
  | Clear(list(t))
  | ResetIndex
  | OnEnter
  | Search(list(t), string)
  | Select(t);

let next = (x, len) => x == len - 1 ? 0 : x + 1;
let prev = (x, len) => x == 0 ? len - 1 : x - 1;

let filterValues = (values, searchText) =>
  searchText === "" ?
    values :
    List.filter(
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
  | Clear(options) => {...state, value: None, filteredValues: options}
  | ResetIndex => {...state, activeIndex: (-1)}
  | Select(choice) => {...state, value: Some(choice), isOpen: false}
  | OnEnter => {
      ...state,
      value: Some(List.nth(state.filteredValues, state.activeIndex)),
      isOpen: false,
    }
  | Search(options, text) => {
      ...state,
      filteredValues: filterValues(options, text),
      activeIndex: (-1),
    }
  };

let initialState = {
  isOpen: false,
  value: None,
  activeIndex: (-1),
  filteredValues: [],
};

/* temp3.getElementsByClassName("menu-item--active")[0].scrollIntoView(false) */

[@bs.send.pipe: Dom.element]
external getElementsByClassName: string => Dom.htmlCollection = "";

[@bs.get] external nextElementSibling: Dom.element => Dom.element = "";

[@bs.send.pipe: Dom.element]
external scrollIntoViewIfNeeded: bool => unit = "";

[@bs.send.pipe: Dom.htmlCollection] [@bs.return nullable]
external item: int => option(Dom.element) = "item";

[@react.component]
let make = (~values: list(t)=[], ~onChange: option(t) => unit=?) => {
  let menuRef = React.useRef(Js.Nullable.null);
  let lastKeyPress = React.useRef("");
  let didMountRef = React.useRef(false);

  let (state, dispatch) = React.useReducer(reducer, initialState);

  React.useEffect1(
    () => {
      dispatch(Search(values, ""));
      Some(() => ());
    },
    [|values|],
  );

  /* skips first render */
  React.useEffect1(
    () => {
      if (React.Ref.current(didMountRef)) {
        onChange(state.value);
      } else {
        React.Ref.setCurrent(didMountRef, true);
      };
      Some(() => ());
    },
    [|state.value|],
  );

  React.useLayoutEffect1(
    () => {
      let menuDiv = menuRef |> React.Ref.current;
      let isScrollingUp = React.Ref.current(lastKeyPress) === "up";

      if (menuDiv !== Js.Nullable.null && state.activeIndex !== (-1)) {
        menuDiv
        |> Js.Nullable.toOption
        |> Belt.Option.getExn
        |> getElementsByClassName("menu-item active")
        |> item(0)
        |> Belt.Option.getExn
        |> scrollIntoViewIfNeeded(isScrollingUp);
        ();
      };

      Some(() => ());
    },
    [|state.activeIndex|],
  );

  let handleSelect = index => {
    let value = List.nth(state.filteredValues, index);
    dispatch(Select(value));
    onChange(Some(value));
  };

  let handleCallback = e =>
    switch (ReactEvent.Keyboard.key(e)) {
    | "ArrowUp" =>
      React.Ref.setCurrent(lastKeyPress, "up");
      dispatch(Prev);
    | "ArrowDown" =>
      React.Ref.setCurrent(lastKeyPress, "down");
      dispatch(Next);
    | "Enter" => dispatch(OnEnter)
    | _ => Js.log("")
    };

  React.useEffect1(
    () => {
      if (state.isOpen) {
        dispatch(ResetIndex);
        DomUtils.addKeybordEventListener("keydown", handleCallback);
      } else {
        DomUtils.removeKeybordEventListener("keydown", handleCallback);
      };
      Some(
        () => DomUtils.removeKeybordEventListener("keydown", handleCallback),
      );
    },
    [|state.isOpen|],
  );

  let handleClear = e => {
    ReactEvent.Mouse.stopPropagation(e);
    dispatch(Clear(values));
    onChange(None);
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
          {
            (
              switch (state.value) {
              | Some(option) => option.label
              | None => "select country"
              }
            )
            |> React.string
          }
        </div>
        {
          switch (state.value) {
          | Some(_) => <Icon.Close onClick=handleClear />
          | None => React.null
          }
        }
        <Icon.ArrowDown />
      </div>
      {
        state.isOpen ?
          <div className="dropdown">
            <div className="search">
              <Icon.Search />
              <input placeholder="Search" onChange=handleSearch />
            </div>
            <div className="menu" ref={ReactDOMRe.Ref.domRef(menuRef)}>
              {
                state.filteredValues
                |> List.mapi((i, x: t) =>
                     <div
                       key={string_of_int(i)}
                       className={
                         "menu-item"
                         ++ (i === state.activeIndex ? " active" : "")
                       }
                       onClick={_ => dispatch(Select(x))}>
                       <span className={"flag-icon flag-icon-" ++ x.value} />
                       <span className="menu-item-label">
                         {React.string(x.label)}
                       </span>
                     </div>
                   )
                |> Array.of_list
                |> React.array
              }
            </div>
          </div> :
          React.null
      }
    </div>
  </div>;
};
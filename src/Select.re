open Hooks;
open VirtualList;
Utils.require("./style.css");
Utils.require("flag-icon-css/css/flag-icon.css");

type t = {
  value: string,
  label: string,
};

type state = {
  isOpen: bool,
  value: option(t),
  values: list(t),
  activeIndex: int,
  filteredValues: list(t),
};

type action =
  | Next
  | Prev
  | Hide
  | ToggleOpen
  | Clear
  | ResetIndex
  | SelectEnter
  | UpdateValues(list(t))
  | Search(string)
  | Select(t);

let convertStyleObjToReasonStyle = x =>
  ReactDOMRe.Style.make()
  |> ReactDOMRe.Style.unsafeAddProp(_, "position", positionGet(x))
  |> ReactDOMRe.Style.unsafeAddProp(_, "left", leftGet(x))
  |> ReactDOMRe.Style.unsafeAddProp(_, "top", topGet(x))
  |> ReactDOMRe.Style.unsafeAddProp(_, "width", widthGet(x));

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
        state.activeIndex + 1 === List.length(state.filteredValues) ?
          state.activeIndex : state.activeIndex + 1,
    }
  | Prev => {
      ...state,
      activeIndex:
        state.activeIndex <= 0 ? state.activeIndex : state.activeIndex - 1,
    }
  | UpdateValues(values) => {...state, values, filteredValues: values}
  | ToggleOpen => {
      ...state,
      isOpen: !state.isOpen,
      filteredValues: state.values,
    }
  | Hide => {...state, isOpen: false, filteredValues: state.values}
  | Clear => {...state, value: None, filteredValues: state.values}
  | ResetIndex => {...state, activeIndex: (-1)}
  | Select(choice) => {
      ...state,
      value: Some(choice),
      filteredValues: state.values,
      isOpen: false,
    }
  | SelectEnter => {
      ...state,
      value: Some(List.nth(state.filteredValues, state.activeIndex)),
      filteredValues: state.values,
      isOpen: false,
    }
  | Search(text) => {
      ...state,
      filteredValues: filterValues(state.values, text),
      activeIndex: (-1),
    }
  };

let initialState = {
  isOpen: false,
  value: None,
  values: [],
  activeIndex: (-1),
  filteredValues: [],
};

[@bs.send.pipe: Dom.element]
external getElementsByClassName: string => Dom.htmlCollection = "";

[@bs.get] external nextElementSibling: Dom.element => Dom.element = "";

[@bs.send.pipe: Dom.element]
external scrollIntoViewIfNeeded: bool => unit = "";

[@bs.send.pipe: Dom.htmlCollection] [@bs.return nullable]
external item: int => option(Dom.element) = "item";

module Country = {
  [@react.component]
  let make = (~active, ~label, ~value, ~style, ~onClick) =>
    <div style className={"menu-item" ++ (active ? " active" : "")} onClick>
      <span className={"flag-icon flag-icon-" ++ value} />
      <span className="menu-item-label"> {React.string(label)} </span>
    </div>;
};

[@react.component]
let make = (~values: list(t)=[], ~onChange: option(t) => unit=?) => {
  let lastKeyPress = React.useRef("");
  let didMountRef = React.useRef(false);
  let (state, dispatch) = React.useReducer(reducer, initialState);

  let divRef = useClickOutside(_ => dispatch(Hide));

  React.useEffect1(
    () => {
      dispatch(UpdateValues(values));
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
      let menuDiv = divRef |> React.Ref.current;
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

  let handleCallback = e =>
    switch (ReactEvent.Keyboard.key(e)) {
    | "ArrowUp" =>
      React.Ref.setCurrent(lastKeyPress, "up");
      dispatch(Prev);
    | "ArrowDown" =>
      React.Ref.setCurrent(lastKeyPress, "down");
      dispatch(Next);
    | "Enter" => dispatch(SelectEnter)
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
    dispatch(Clear);
    onChange(None);
  };

  let handleSearch = e => {
    let value = ReactEvent.Form.target(e)##value |> String.lowercase;
    dispatch(Search(value));
  };

  let filteredValues = state.filteredValues |> Array.of_list;
  let filteredValuesLength = Array.length(filteredValues);

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
            <VirtualList
              height={
                filteredValuesLength > 15 ? 400 : filteredValuesLength * 28
              }
              itemCount=filteredValuesLength
              itemSize=28
              width=298>
              ...{
                   (~props) => {
                     let style =
                       props |> styleGet |> convertStyleObjToReasonStyle;
                     let index = props |> indexGet;
                     <Country
                       style
                       key={string_of_int(index)}
                       active={index == state.activeIndex}
                       value={filteredValues[index].value}
                       label={filteredValues[index].label}
                       onClick={_ => dispatch(Select(filteredValues[index]))}
                     />;
                   }
                 }
            </VirtualList>
          </div> :
          React.null
      }
    </div>
  </div>;
};
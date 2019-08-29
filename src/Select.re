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
  | OnEnter
  | UpdateValues(list(t))
  | Search(string)
  | Select(t);

let convertStyleObjToReasonStyle = x =>
  ReactDOMRe.Style.make()
  |> ReactDOMRe.Style.unsafeAddProp(_, "position", positionGet(x))
  |> ReactDOMRe.Style.unsafeAddProp(_, "left", leftGet(x))
  |> ReactDOMRe.Style.unsafeAddProp(_, "top", topGet(x))
  |> ReactDOMRe.Style.unsafeAddProp(_, "width", widthGet(x));

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
  | UpdateValues(values) => {...state, values, filteredValues: values}
  | ToggleOpen => {
      ...state,
      isOpen: !state.isOpen,
      filteredValues: state.values,
    }
  | Hide => {...state, isOpen: false, filteredValues: state.values}
  | Clear => {...state, value: None, filteredValues: state.values}
  | ResetIndex => {...state, activeIndex: (-1)}
  | Select(choice) => {...state, value: Some(choice), isOpen: false}
  | OnEnter => {
      ...state,
      value: Some(List.nth(state.filteredValues, state.activeIndex)),
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
  let didMountRef = React.useRef(false);

  let (state, dispatch) = React.useReducer(reducer, initialState);

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
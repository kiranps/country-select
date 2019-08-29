[@bs.deriving abstract]
type vstyle = {
  position: string,
  left: string,
  top: string,
  width: string,
};

[@bs.deriving abstract]
type t = {
  index: int,
  style: vstyle,
};

[@bs.module "react-window"] [@react.component]
external make:
  (
    ~height: int,
    ~itemCount: int,
    ~itemSize: int,
    ~width: int,
    ~children: (~props: t) => React.element
  ) =>
  React.element =
  "FixedSizeList";
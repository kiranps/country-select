[@bs.module "react-select"] [@react.component]
external make:
  (
    ~value: option('a),
    ~options: array('a),
    ~onChange: Js.nullable('a) => unit
  ) =>
  React.element =
  "default";
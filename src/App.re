open Js.Promise;

[@react.component]
let make = () => {
  let handleChange = e => Js.log(e);

  <Select country={Some("us")} onChange=handleChange />;
};
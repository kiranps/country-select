open Js.Promise;
Utils.require("./app.css");

[@react.component]
let make = () => {
  let handleChange = e => Js.log(e);

  <div className="app">
    <Select country={Some("us")} onChange=handleChange />
    <Select country={Some("us")} onChange=handleChange />
    <Select country={Some("us")} onChange=handleChange />
  </div>;
};
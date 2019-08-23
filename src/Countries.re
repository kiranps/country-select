module Decode = {
  open Select;

  let object_ = json =>
    Json.Decode.{
      value: json |> field("value", string),
      label: json |> field("label", string),
    };

  let objectList = Json.Decode.list(object_);
};

module Db = {
  open Js.Promise;

  let fetch = () =>
    Http.get("countries.json")
    |> then_(json => json |> Decode.objectList |> resolve);
};
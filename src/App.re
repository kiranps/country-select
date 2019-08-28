open Js.Promise;

[@react.component]
let make = () => {
  let (choices, setChoices) = React.useState(_ => []);

  React.useEffect0(() => {
    let _ =
      Countries.Db.fetch()
      |> then_(values => setChoices(_ => values) |> resolve);
    Some(() => ());
  });

  let handleChange = e => Js.log(e);

  <Select values=choices onChange=handleChange />;
};
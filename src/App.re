open Js.Promise;

/* module Item = {
     [@react.component]
     let make = (~style) => <div style> {React.string("hello")} </div>;
   }; */

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

/* <VirtualList height=300 itemCount=400 itemSize=28 width=100>
     ...{
          (~props) => {
            let style = convertStyleObjToReasonStyle(props->style);
            <Item style />;
          }
        }
   </VirtualList>; */
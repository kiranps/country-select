module Search = {
  [@react.component]
  let make = () =>
    <svg viewBox="0 0 24 24">
      <path
        fill="#000000"
        d="M9.5,3A6.5,6.5 0 0,1 16,9.5C16,11.11 15.41,12.59 14.44,13.73L14.71,14H15.5L20.5,19L19,20.5L14,15.5V14.71L13.73,14.44C12.59,15.41 11.11,16 9.5,16A6.5,6.5 0 0,1 3,9.5A6.5,6.5 0 0,1 9.5,3M9.5,5C7,5 5,7 5,9.5C5,12 7,14 9.5,14C12,14 14,12 14,9.5C14,7 12,5 9.5,5Z"
      />
    </svg>;
};

module Close = {
  [@react.component]
  let make = (~onClick) =>
    <svg viewBox="0 0 24 24" className="icon-close" onClick>
      <path
        fill="#000000"
        d="M13.46,12L19,17.54V19H17.54L12,13.46L6.46,19H5V17.54L10.54,12L5,6.46V5H6.46L12,10.54L17.54,5H19V6.46L13.46,12Z"
      />
    </svg>;
};

module ArrowDown = {
  [@react.component]
  let make = () =>
    <svg viewBox="0 0 24 24" className="icon-arrow-down">
      <path fill="#000000" d="M7,10L12,15L17,10H7Z" />
    </svg>;
};
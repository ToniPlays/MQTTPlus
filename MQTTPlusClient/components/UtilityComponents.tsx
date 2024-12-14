import { useTimeAgo } from "react-time-ago";
import { Capitalize } from "./Utility";

export const TimeSince = (props: { time: any}) => {
    const since = useTimeAgo({
      date: new Date(props.time ?? 0),
      locale: "en",
    });
  
    return (<p>
      {Capitalize(since.formattedDate)}
    </p>)
  }
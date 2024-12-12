
export const ConnectionStatus = (props: { status: number}) => {
    const { status } = props
    return (<p className={status ? "text-meta-3" : "text-meta-1"}>
        {status ? "Connected" : "Disconnected"}
    </p>)
}
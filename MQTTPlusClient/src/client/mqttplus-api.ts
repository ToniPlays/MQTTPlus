
export interface ExpandOpts {
    expands?: Array<string>
}

export default class MQTTPlusAPI {
    
    server(opts?: ExpandOpts) {
        return {
            endpoint: '/server',
            opts: opts,
        }
   }
   event(data: {listen: string[] })
   {
    return {
        endpoint: '/event',
        listen: data.listen,
    }
   }
}


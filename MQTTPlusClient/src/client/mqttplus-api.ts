
export interface ExpandOpts {
    expands?: Array<string>
}

export default class MQTTPlusAPI {
   
    devices(expand?: ExpandOpts)
    {
        return {
            endpoint: '/devices',
            opts: expand,
        }
    }

    server(expand?: ExpandOpts) {
        return {
            endpoint: '/server',
            opts: expand,
        }
   }
   
   event(data: { listen: string[] })
   {
    return {
        endpoint: '/event',
        listen: data.listen,
    }
   }
}


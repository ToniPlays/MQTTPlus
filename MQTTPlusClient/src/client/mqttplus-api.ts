
import './types/Server'
import './types/Events'
import { MQTTPlus as Server } from './types/Server'
import { MQTTPlus as Events} from './types/Events'
import { MQTTPlus as Devices} from './types/Devices'

export default class MQTTPlusAPI {
    server = new Server.Server()
    devices = new Devices.Devices()
    events = new Events.Events()
}


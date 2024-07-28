
import './types/Server'
import './types/Events'
import { MQTTPlus as Server } from './types/Server'
import { MQTTPlus as Events} from './types/Events'

export default class MQTTPlusAPI {
    server = new Server.Server()
    events = new Events.Events()
}


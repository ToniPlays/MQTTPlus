
import axios from 'axios'

const url = 'http://80.95.140.80:8883'

const client = axios.create({
	baseUrl: url
})

export default client
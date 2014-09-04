//Based largely on Pebble Hacker News by Neal
var items = {};

var ENDPOINTS = {
	PLAYERS: 0,
	ATTACKS: 1,
	TBC1: 2,
	TBC2: 3
};

var BASE_URL = 'http://www.mirz.com/vandalizr.io/?method=';

var API_URLS = {
	[ENDPOINTS.PLAYERS]: BASE_URL + 'getPlayers',
	[ENDPOINTS.ATTACKS]: BASE_URL + 'getAttacks',
	[ENDPOINTS.TBC1]: BASE_URL + 'tbc1',
	[ENDPOINTS.TBC2]: BASE_URL + 'tbc2'
};


var appMessageQueue = {
	queue: [],
	numTries: 0,
	maxTries: 5,
	working: false,
	clear: function() {
		this.queue = [];
		this.working = false;
	},
	isEmpty: function() {
		return this.queue.length === 0;
	},
	nextMessage: function() {
		return this.isEmpty() ? {} : this.queue[0];
	},
	send: function(message) {
		if (message) this.queue.push(message);
		if (this.working) return;
		if (this.queue.length > 0) {
			this.working = true;
			var ack = function() {
				appMessageQueue.numTries = 0;
				appMessageQueue.queue.shift();
				appMessageQueue.working = false;
				appMessageQueue.send();
			};
			var nack = function() {
				appMessageQueue.numTries++;
				appMessageQueue.working = false;
				appMessageQueue.send();
			};
			if (this.numTries >= this.maxTries) {
				console.log('Failed sending AppMessage: ' + JSON.stringify(this.nextMessage()));
				ack();
			}
			//console.log('Sending AppMessage: ' + JSON.stringify(this.nextMessage()));
			Pebble.sendAppMessage(this.nextMessage(), ack, nack);
		}
	}
};


function getItems(endpoint) {
	var xhr = new XMLHttpRequest();
	xhr.open('GET', API_URLS[endpoint], true);
	xhr.timeout = 20000;
	xhr.onload = function(e) {
		if (xhr.readyState == 4) {
			if (xhr.status == 200) {
				if (xhr.responseText) {
					var res = JSON.parse(xhr.responseText);
					var items = res.items;
					for(var index = 0; index < res.length; index++) {
                        var id = decodeURIComponent(res[index].id);
						var name = decodeURIComponent(res[index].name);
						appMessageQueue.send({'endpoint': endpoint, 'index': index, 'id': id, 'name': name});
					}
				} else {
					console.log('Invalid response received! ' + JSON.stringify(xhr));
					appMessageQueue.send({'endpoint': endpoint, 'name': 'Invalid response!'});
				}
			} else {
				console.log('Request returned error code ' + xhr.status.toString());
				appMessageQueue.send({'endpoint': endpoint, 'name': 'HTTP/1.1 ' + xhr.statusText});
			}
		}
	}
	xhr.ontimeout = function() {
		console.log('HTTP request timed out');
		appMessageQueue.send({'endpoint': endpoint, 'name': 'Request timed out!'});
	};
	xhr.onerror = function() {
		console.log('HTTP request return error');
		appMessageQueue.send({'endpoint': endpoint, 'name': 'Failed to connect!'});
	};
	xhr.send(null);
}


Pebble.addEventListener('ready', function(e) {
   //console.log('JS Ready');
});

Pebble.addEventListener('appmessage', function(e) {
	//console.log('AppMessage received from Pebble: ' + JSON.stringify(e.payload));
	if (typeof(e.payload.endpoint) != 'undefined') {
		getItems(e.payload.endpoint);
	} else {
		appMessageQueue = [];
	}
});

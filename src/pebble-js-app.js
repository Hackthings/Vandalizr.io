//Based largely on Pebble Hacker News by Neal

var options = {
	appMessage: {
		maxBuffer: 480,
		maxTries: 3,
		retryTimeout: 3000,
		timeout: 100
	},
	http: {
		timeout: 20000
	}
};

var appMessageQueue = [];
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

function sendAppMessageQueue() {
	if (appMessageQueue.length > 0) {
		currentAppMessage = appMessageQueue[0];
		currentAppMessage.numTries = currentAppMessage.numTries || 0;
		currentAppMessage.transactionId = currentAppMessage.transactionId || -1;
		if (currentAppMessage.numTries < options.appMessage.maxTries) {
			console.log('Sending AppMessage to Pebble: ' + JSON.stringify(currentAppMessage.message));
			Pebble.sendAppMessage(
				currentAppMessage.message,
				function(e) {
					appMessageQueue.shift();
					setTimeout(function() {
						sendAppMessageQueue();
					}, options.appMessage.timeout);
				}, function(e) {
					console.log('Failed sending AppMessage for transactionId:' + e.data.transactionId + '. Error: ' + e.data.error.message);
					appMessageQueue[0].transactionId = e.data.transactionId;
					appMessageQueue[0].numTries++;
					setTimeout(function() {
						sendAppMessageQueue();
					}, options.appMessage.retryTimeout);
				}
			);
		} else {
			appMessageQueue.shift();
			console.log('Failed sending AppMessage for transactionId:' + currentAppMessage.transactionId + '. Bailing. ' + JSON.stringify(currentAppMessage.message));
		}
	} else {
		console.log('AppMessage queue is empty.');
	}
}

function getItems(endpoint) {
	var xhr = new XMLHttpRequest();
	xhr.open('GET', API_URLS[endpoint], true);
	xhr.timeout = options.http.timeout;
	xhr.onload = function(e) {
		if (xhr.readyState == 4) {
			if (xhr.status == 200) {
				if (xhr.responseText) {
					var res = JSON.parse(xhr.responseText);
					var items = res.items;
					/*
					items.forEach(function (element, index, array) {
                        var id = decodeURIComponent(element.id);
						var name = decodeURIComponent(element.name);
						appMessageQueue.push({'message': {'endpoint': endpoint, 'index': index, 'id': id, 'name': name}});
					});
					*/
					for(var index = 0; index < res.length; index++) {
                        var id = decodeURIComponent(res[index].id);
						var name = decodeURIComponent(res[index].name);
						appMessageQueue.push({'message': {'endpoint': endpoint, 'index': index, 'id': id, 'name': name}});
					}
				} else {
					console.log('Invalid response received! ' + JSON.stringify(xhr));
					appMessageQueue.push({'message': {'endpoint': endpoint, 'name': 'Invalid response!'}});
				}
			} else {
				console.log('Request returned error code ' + xhr.status.toString());
				appMessageQueue.push({'message': {'endpoint': endpoint, 'name': 'HTTP/1.1 ' + xhr.statusText}});
			}
		}
		sendAppMessageQueue();
	}
	xhr.ontimeout = function() {
		console.log('HTTP request timed out');
		appMessageQueue.push({'message': {'endpoint': endpoint, 'name': 'Request timed out!'}});
		sendAppMessageQueue();
	};
	xhr.onerror = function() {
		console.log('HTTP request return error');
		appMessageQueue.push({'message': {'endpoint': endpoint, 'name': 'Failed to connect!'}});
		sendAppMessageQueue();
	};
	xhr.send(null);
}


Pebble.addEventListener('ready', function(e) {
    console.log('JS Ready');
});

Pebble.addEventListener('appmessage', function(e) {
	console.log('AppMessage received from Pebble: ' + JSON.stringify(e.payload));
	if (typeof(e.payload.endpoint) != 'undefined') {
		getItems(e.payload.endpoint);
	} else {
		appMessageQueue = [];
	}
});

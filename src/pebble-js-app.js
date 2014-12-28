function playerName() {
	//TODO: Config setting
	return '';
}

//appMessageQueue from Neal
//https://github.com/Neal/Ultra/blob/master/src/js/src/appmessagequeue.js

var items = {};

var ENDPOINTS = {
	PLAYERS: 0,
	ATTACKS: 1,
	PLAYERCREATE: 2,
	PLAYERDELETE: 3,
	GAMECREATE: 4,
	GAMEACTION: 5,
	CHECKSTATUS: 6
};

var BASE_URL = 'http://www.mirz.com/vandalizr.io/?method=';



var TRANSFER_IN_PROGRESS = false;

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
				//console.log('Failed sending AppMessage: ' + JSON.stringify(this.nextMessage()));
				ack();
			}
			//console.log('Sending AppMessage: ' + JSON.stringify(this.nextMessage()));
			Pebble.sendAppMessage(this.nextMessage(), ack, nack);
		}
	}
};

var API_URLS = {
	[ENDPOINTS.PLAYERS]: BASE_URL + 'playerList',
	[ENDPOINTS.ATTACKS]: BASE_URL + 'attackList',
	[ENDPOINTS.PLAYERCREATE]: BASE_URL + 'playerCreate',
	[ENDPOINTS.PLAYERDELETE]: BASE_URL + 'playerDelete',
	[ENDPOINTS.GAMECREATE]: BASE_URL + 'gameCreate',
	[ENDPOINTS.GAMEACTION]: BASE_URL + 'gameActionCreate',
	[ENDPOINTS.CHECKSTATUS]: BASE_URL + 'gameStatus',
};

function getItems(endpoint, data) {
	var xhr = new XMLHttpRequest();
	var url;
	switch(endpoint) {
		case ENDPOINTS.PLAYERS:
			url = API_URLS[endpoint] + '&playerid=' + Pebble.getAccountToken();
			break;
		case ENDPOINTS.ATTACKS:
			url = API_URLS[endpoint];
			break;
		case ENDPOINTS.PLAYERCREATE:
			url = API_URLS[endpoint] + '&playerid=' + Pebble.getAccountToken() + '&playername=' + playerName();
			break;
		case ENDPOINTS.PLAYERDELETE:
			url = API_URLS[endpoint] + '&playerid=' + Pebble.getAccountToken();
			break;
		case ENDPOINTS.GAMECREATE:
			url = API_URLS[endpoint] + '&attackerid=' + Pebble.getAccountToken() + '&victimid=' + data.payload.victim_id + '&game=1';
			break;
		case ENDPOINTS.GAMEACTION:
			url = API_URLS[endpoint] + '&playerid=' + Pebble.getAccountToken()  + '&gameid=' + data.payload.game_id + '&action=' + data.payload.action_id;
			break;
		case ENDPOINTS.CHECKSTATUS:
				var gameid;
				if(data.payload.game_id) {
					gameid = data.payload.game_id;
				}
				url = API_URLS[endpoint] + '&playerid=' + Pebble.getAccountToken()  + '&gameid=' + gameid;
			break;
	}
	
	xhr.open('GET', url, true);
	xhr.timeout = 20000;
	xhr.onload = function(e) {
		if (xhr.readyState == 4) {
			if (xhr.status == 200) {
				if (xhr.responseText) {
					var res = JSON.parse(xhr.responseText);
					console.log('res: ' + JSON.stringify(res));
					var items = res.items;
					for(var index = 0; index < res.length; index++) {
            var id = decodeURIComponent(res[index].id);
						console.log('JS ID: ' + id + ' - JS name:' + name + ' - index: ' + index);
						var name = decodeURIComponent(res[index].name);
						
						
						if(endpoint == ENDPOINTS.CHECKSTATUS) {
							
							id = parseInt(id);
							index = parseInt(index);
							name = parseInt(name);
							
						}
						

						appMessageQueue.send({'endpoint': endpoint, 'index': index, 'id': id, 'name': name});
					}
				} else {
					//console.log('Invalid response received! ' + JSON.stringify(xhr));
					appMessageQueue.send({'endpoint': endpoint, 'name': 'Invalid response!'});
				}
			} else {
				//console.log('Request returned error code ' + xhr.status.toString());
				appMessageQueue.send({'endpoint': endpoint, 'name': 'HTTP/1.1 ' + xhr.statusText});
			}
		}
	}
	xhr.ontimeout = function() {
		//console.log('HTTP request timed out');
		appMessageQueue.send({'endpoint': endpoint, 'name': 'Request timed out!'});
	};
	xhr.onerror = function() {
		//console.log('HTTP request return error');
		appMessageQueue.send({'endpoint': endpoint, 'name': 'Failed to connect!'});
	};
	xhr.send(null);
}


Pebble.addEventListener('ready', function(e) {
   //console.log('JS Ready');
});

Pebble.addEventListener('appmessage', function(e) {
	console.log('AppMessage received from Pebble: ' + JSON.stringify(e.payload));
	if (typeof(e.payload.endpoint) != 'undefined') {
		getItems(e.payload.endpoint, e);
		return;
	} else {
		appMessageQueue = [];
	}
	
	if ('NETIMAGE_URL' in e.payload) {
		if (TRANSFER_IN_PROGRESS == false) {
			TRANSFER_IN_PROGRESS = true;
			downloadBinaryResource(e.payload['NETIMAGE_URL'], function(bytes) {
				transferImageBytes(bytes, e.payload['NETIMAGE_CHUNK_SIZE'],
					function() { console.log("Done!"); TRANSFER_IN_PROGRESS = false; },
					function(e) { console.log("Failed! " + JSON.stringify(e)); TRANSFER_IN_PROGRESS = false; }
				);
			});
		}
		else {
			console.log("Ignoring request to download " + e.payload['NETIMAGE_URL'] + " because another download is in progress.");
		}
	}
});



// Pebble Faces from Sarfata
// https://github.com/pebble-hacks/pebble-faces

function downloadBinaryResource(imageURL, callback, errorCallback) {
    var req = new XMLHttpRequest();
    req.open("GET", imageURL,true);
    req.responseType = "arraybuffer";
    req.onload = function(e) {
        console.log("loaded");
        var buf = req.response;
        if(req.status == 200 && buf) {
            var byteArray = new Uint8Array(buf);
            var arr = [];
            for(var i=0; i<byteArray.byteLength; i++) {
                arr.push(byteArray[i]);
            }

            console.log("Received image with " + byteArray.length + " bytes.");
            callback(arr);
        }
        else {
          errorCallback("Request status is " + req.status);
        }
    }
    req.onerror = function(e) {
      errorCallback(e);
    }
    req.send(null);
}

function transferImageBytes(bytes, chunkSize, successCb, failureCb) {
  var retries = 0;

  success = function() {
    console.log("Success cb=" + successCb);
    if (successCb != undefined) {
      successCb();
    }
  };
  failure = function(e) {
    console.log("Failure cb=" + failureCb);
    if (failureCb != undefined) {
      failureCb(e);
    }
  };

  // This function sends chunks of data.
  sendChunk = function(start) {
    var txbuf = bytes.slice(start, start + chunkSize);
    console.log("Sending " + txbuf.length + " bytes - starting at offset " + start);
    Pebble.sendAppMessage({ "NETIMAGE_DATA": txbuf },
      function(e) {
        // If there is more data to send - send it.
        if (bytes.length > start + chunkSize) {
          sendChunk(start + chunkSize);
        }
        // Otherwise we are done sending. Send closing message.
        else {
          Pebble.sendAppMessage({"NETIMAGE_END": "done" }, success, failure);
        }
      },
      // Failed to send message - Retry a few times.
      function (e) {
        if (retries++ < 3) {
          console.log("Got a nack for chunk #" + start + " - Retry...");
          sendChunk(start);
        }
        else {
          failure(e);
        }
      }
    );
  };

  // Let the pebble app know how much data we want to send.
  Pebble.sendAppMessage({"NETIMAGE_BEGIN": bytes.length },
    function (e) {
      // success - start sending
      sendChunk(0);
    }, failure);

}
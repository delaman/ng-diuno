'use strict';

angular.module('myApp')
  .directive('mainView', function($timeout, HOSTED_URL, Arduino) {
    return {
      templateUrl: HOSTED_URL() + '/views/home.html',
      restrict: 'A',
      link: function(scope) {
        var redPin = 9,
            greenPin = 8,
            temp = 7;

        var selectPin = function(n, pins) {
          for (var i = pins.length - 1; i >= 0; i--) {
            if (pins[i].pin === n) {
              return pins[i];
            }
          }
        };

        scope.refresh = function() {
          (function poll() {
            Arduino.setPins([
              { pin: temp, action: 'getTemp' }
            ])
            .then(function(data) {
              var pins = data.pins;
              var pin = selectPin(temp, pins);
              scope.temperature = Math.round(pin.value * 100) / 100;
              $timeout(poll, 1000);
            });
          })();
        };

        scope.lowerTemp = function() {
          $timeout(function() {
            scope.loweringTemp = true;
            $timeout(function() {
              scope.loweringTemp = false;
            }, 800);
          }, 100);
          Arduino.setPins([
            { pin: greenPin, value: 1 }
          ]).then(function() {
            $timeout(function() {
              Arduino.setPins([
                {pin: greenPin, value: 0}
              ]);
            }, 1000);
          });
        };

        scope.raiseTemp = function() {
          $timeout(function() {
            scope.raisingTemp = true;
            $timeout(function() {
              scope.raisingTemp = false;
            }, 800);
          }, 100);
          Arduino.setPins([
            { pin: redPin, value: 1 }
          ]).then(function() {
            $timeout(function() {
              Arduino.setPins([
                {pin: redPin, value: 0}
              ]);
            }, 1000);
          });
        };

        scope.refresh(); // Kick it off
      }
    };
  });

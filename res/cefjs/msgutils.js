"use strict";

/* 消息id */
var msgId = (function () {
    var value = 0;
    return {
        get: function () {
            return ++value;
        }
    }
})();

/* 消息id对应成功和失败函数 */
var callbackList = (function () {
    var list = {};

    return {
        addEvent: function (msg) {
            // msg {
            //     id:
            //     request:
            //     onSuccess:
            //     onFailure:
            // }
            msg["id"] =msgId.get();
            list[msg.request] = {
                id:msg.id,
                onSuccess: msg.onSuccess,
                onFailure: msg.onFailure
            };
        },

        /*  */
        execEvent: function (msg) {
            // 返回值为json,如下
            // msg {
            //     id:
            //     status:
            //     successMsg:
            //     failureVal:
            //     failureMsg:
            // }
            var obj = list[msg.request];
            if (obj === undefined) {
                return;
            }
            if (msg.status === "success") {
                obj.onSuccess(msg.successMsg);
            } else if (msg.status === "failure") {
                obj.onFailure(msg.failureVal,msg.failureMsg);
            } else {
                obj.failure(msg.failureVal, msg.failureMsg);
            }
            list[msg.request] = undefined;
        }
    }
})();


window.qtQuery = function(msg) {
    callbackList.addEvent(msg);
    // qmlObject.callQML(msg);
    window.sendMessage(msg);
}
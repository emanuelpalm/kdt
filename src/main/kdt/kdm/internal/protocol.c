#include "protocol.h"
#include "message.h"
#include <errno.h>
#include <kdt/kvs.h>
#include <kdt/log.h>
#include <kdt/pnet/pnet.h>

#define _TRY(ERR) do {         \
    const err_t _err = (ERR); \
    if (_err != ERR_NONE) {   \
        return _err;          \
     }                        \
} while (0)

err_t _kdm_InitProtocol(_kdm_Protocol *protocol, kvs_t *store, pnet_t *pnet) {
    // Load or generate client ID.
    {
        kint_t zero = {0};
        mem_t out = mem_FromBuffer((uint8_t *) &protocol->id, sizeof(kint_t));
        err_t err = kvs_Get(store, &zero, &out);
        switch (err) {
        case ERR_NONE:
            break;

        case ERR_NOT_FOUND:
            protocol->id = kint_Random();
            _TRY(kvs_Set(store, &zero, sizeof(kint_t), (uint8_t *) &protocol->id));
            break;

        default:
            return err;
        }
    }

    _kdm_InitTable(&protocol->table, &protocol->id);
    protocol->store = store;
    protocol->pnet = pnet;

    return ERR_NONE;
}

inline
kint_t *_kdm_GetClientID(_kdm_Protocol *protocol) {
    return &protocol->id;
}

err_t _kdm_Join(_kdm_Protocol *protocol, const pnet_Host *peer) {
    pnet_Message *message = pnet_NewMessage(protocol->pnet);
    if (message == NULL) {
        return ENOMEM;
    }
    message->nonce = kint_Random();
    message->tag = _KDM_MESSAGE_TAG_FIND_NODE;
    message->receiver = *peer;

    // Sender ID.
    mem_Write(&message->data, &protocol->id, sizeof(kint_t));

    // Searched ID.
    mem_Write(&message->data, &protocol->id, sizeof(kint_t));

    return pnet_Send(protocol->pnet, message);
}

// TODO: Add _kdm_Get() and _kdm_Set() functions, for getting and setting network data.

void LogError(pnet_EventError *error);

err_t _kdm_Poll(_kdm_Protocol *protocol) {
    pnet_Event *event = NULL;
    _TRY(pnet_Poll(protocol->pnet, &event));
    if (event == NULL) {
        return ERR_NOT_FOUND;
    }
    switch (event->as_type) {
    case PNET_EVENT_ERROR:
        LogError(&event->as_error);
        break;

    case PNET_EVENT_MESSAGE:
        log_Note("Message received!"); // TODO: Handle message.
        break;

    default:
        log_BugF("Unexpected event type: %d", event->as_type);
        break;
    }
    pnet_FreeEvent(protocol->pnet, event);
    return ERR_NONE;
}

void LogError(pnet_EventError *error) {
    char _text[128];
    _text[sizeof(_text) - 1] = '\0';
    mem_t text = mem_FromBuffer((uint8_t *) _text, sizeof(_text) - 1);

    const char *nonce = (const char *) text.offset;
    kint_WriteText(&error->nonce, &text);
    mem_Write8(&text, '\0');

    const char *host = (const char *) text.offset;
    pnet_WriteHostText(&error->host, &text);
    mem_Write8(&text, '\0');

    log_WarnF(
        "Network Error\n"
        "  Message: %s\n"
        "  Code:    %d\n"
        "  Tag:     %s\n"
        "  Nonce:   %s\n"
        "  Host:    %s",
        err_GetDescription(error->code),
        error->code,
        _kdm_MessageTagAsString(error->tag),
        nonce,
        host
    );
}
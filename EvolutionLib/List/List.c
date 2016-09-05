//
// Created by Oleg on 12/5/15.
//

#include "List.h"
#include "Logging/Logging.h"

short initList(List* list, comparator c, destructor d) {
    if (!list) {
        return 0;
    }
    list->head = NULL;
    list->tail = NULL;
    list->length = 0;
    list->c = c;
    list->d = d;
    return 1;
}

short clearList(List* list) {
    if (!list) {
        return 0;
    }
    if (!list->length || !list->head) {
        return 1;
    }
    Node* temp;
    while (list->head != NULL) {
        temp = list->head->next;
        if (list->d) {
            list->d(list->head->value);
        }
        free(list->head);
        list->head = temp;
    }
    list->head = NULL;
    list->tail = NULL;
    list->length = 0;
    list->c = NULL;
    list->d = NULL;
    return 1;
}

short clearListPointer(List* list) {
    short result = clearList(list);
    free(list);
    return result;
}

short pushBack(List* list, void* value) {
    if (!list) {
        return 0;
    }
    Node* new = (Node*)malloc(sizeof(Node));
    if (!new) {
        return 0;
    }
    new->value = value;
    new->prev = list->tail;
    new->next = NULL;
    list->tail = new;
    if (new->prev) {
        new->prev->next = new;
    }
    else {
        list->head = new;
    }
    ++list->length;
    return 1;
}

short insert(ListIterator nextIt, void* value) {
    if (!nextIt.list) {
        return 0;
    }
    Node* new = (Node*)malloc(sizeof(Node));
    if (!new) {
        return 0;
    }
    new->value = value;
    new->next = nextIt.current;
    if (new->next) {
        new->prev = new->next->prev;
        if (new->prev) {
            new->prev->next = new;
        }
        new->next->prev = new;
    }
    else {
        new->prev = nextIt.list->tail;
        if (new->prev) {
            new->prev->next = new;
        }
        nextIt.list->tail = new;
    }
    if (!new->prev) {
        nextIt.list->head = new;
    }
    ++nextIt.list->length;
    return 1;
}

short copyList(List* destination, List* source, copier cp) {
    if (!destination || !source) {
        return 0;
    }
    LOG_ASSERT(destination->c == source->c && destination->d == source->d);

    for (Node* it = source->head; it != NULL; it = it->next) {
        void* copy = cp(it->value);
        if (!copy) {
            return 0;
        }
        if (!pushBack(destination, copy)) {
            if (source->d) {
                source->d(copy);
            }
            return 0;
        }
    }
    return 1;
}

short moveList(List* destination, List* source) {
    if (!destination || !source) {
        return 0;
    }
    LOG_ASSERT(destination->c == source->c && destination->d == source->d);

    if (destination->tail) {
        destination->tail->next = source->head;
        source->head->prev = destination->tail;
        destination->tail = source->tail;
    }
    else {
        destination->head = source->head;
        destination->tail = source->tail;
    }
    destination->length += source->length;
    source->head = NULL;
    source->tail = NULL;
    source->length = 0;
    return 1;
}

ListIterator findByVal(List* list, void* value) {
    ListIterator it = {
            list,
            NULL
    };
    if (!list || !list->length || !list->c) {
        return it;
    }
    it.current = list->head;
    while (it.current != NULL) {
        if (list->c(it.current->value, value) == 0) {
            break;
        }
        it.current = it.current->next;
    }
    return it;
}

ListIterator findByIndex(List* list, size_t index) {
    ListIterator it = {
            list,
            NULL
    };
    if (!list || index >= list->length) {
        return it;
    }
    it.current = list->head;
    size_t curr_index = 0;
    while (curr_index < index) {
        it.current = it.current->next;
        ++curr_index;
    }
    return it;
}

short removeByValue(List* list, void* value) {
    ListIterator it = findByVal(list, value);
    return removeFromList(it);
}

short removeByIndex(List* list, size_t index) {
    ListIterator it = findByIndex(list, index);
    return removeFromList(it);
}

short removeFromList(ListIterator it) {
    if (!it.list || !it.current) {
        return 0;
    }
    if (it.current->next) {
        it.current->next->prev = it.current->prev;
    }
    else {
        it.list->tail = it.current->prev;
    }
    if (it.current->prev) {
        it.current->prev->next = it.current->next;
    }
    else {
        it.list->head = it.current->next;
    }
    if (it.list->d) {
        it.list->d(it.current->value);
    }
    free(it.current);
    --it.list->length;
    return 1;
}

ListIterator begin(List* list) {
    ListIterator it = {
            list,
            NULL
    };
    if (!list || !list->length) {
        return it;
    }
    it.current = list->head;
    return it;
}

ListIterator end(List* list) {
    ListIterator it = {
            list,
            NULL
    };
    return it;
}

void next(ListIterator* it) {
    if (it && it->current) {
        it->current = it->current->next;
    }
}

void prev(ListIterator* it) {
    if (it && it->current) {
        it->current = it->current->prev;
    }
}

short isIteratorExhausted(ListIterator it) {
    return it.current == NULL;
}
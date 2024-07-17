#ifndef RANDOM_ACCESS_ARR
#define RANDOM_ACCESS_ARR
#include <cstdint>
#include <cstdlib>
#include <random>
#include <cassert>

template<typename T>
class RandomAccessArr {
  public:
    RandomAccessArr() : size(0), capacity(0), items(nullptr) {}

    ~RandomAccessArr() {
      if (this->items) 
        free(this->items);
    }

    RandomAccessArr(const RandomAccessArr& other) : size(other.size), capacity(other.capacity), items(nullptr) {
      if (capacity > 0) {
        items = static_cast<T*>(malloc(capacity * sizeof(T)));
        for (uint8_t i = 0; i < size; ++i) {
          items[i] = other.items[i];
        }
      }
    }

    RandomAccessArr& operator=(const RandomAccessArr& other) {
      if (this != &other) {
        if (items) 
          free(items);

        size = other.size;
        capacity = other.capacity;
        items = nullptr;

        if (capacity > 0) {
          items = static_cast<T*>(malloc(capacity * sizeof(T)));
          for (uint8_t i = 0; i < size; ++i) {
            items[i] = other.items[i];
          }
        }
      }
      return *this;
    }

    RandomAccessArr(RandomAccessArr&& other) noexcept : size(other.size), capacity(other.capacity), items(other.items) {
      other.size = 0;
      other.capacity = 0;
      other.items = nullptr;
    }

    RandomAccessArr& operator=(RandomAccessArr&& other) noexcept {
      if (this != &other) {
        if (items) 
          free(items);

        size = other.size;
        capacity = other.capacity;
        items = other.items;

        other.size = 0;
        other.capacity = 0;
        other.items = nullptr;
      }
      return *this;
    }

    void Add(T item) {
      if (this->size >= this->capacity) {
        auto new_capacity = 2 * this->capacity + 1;
        if (new_capacity > 32) 
          new_capacity = 32; // at max 32 children

        this->items = static_cast<T*>(realloc(this->items, new_capacity * sizeof(T)));
        this->capacity = new_capacity;
      }

      this->items[this->size++] = item;
    }

    T GetRandomItemAndRemove() {
      assert(this->size > 0);

      static std::random_device rd;
      static std::mt19937 gen(rd());
      std::uniform_int_distribution<> dis(0, this->size - 1);

      int i = dis(gen);
      T item = this->items[i];
      this->items[i] = this->items[size - 1];
      this->size--;

      if (this->size == 0) {
        free(this->items);
        this->items = nullptr;
        this->capacity = 0;
      }

      return item;
    }

    inline void Clear() {
      this->size = 0;
    }

    inline uint8_t Size() const {
      return this->size;
    }

  private:
    uint8_t size;
    uint8_t capacity;
    T *items;
};
#endif // RANDOM_ACCESS_ARR

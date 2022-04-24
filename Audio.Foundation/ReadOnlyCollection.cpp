#include "pch.h"
#include "ReadOnlyCollection.h"

using namespace Audio::Foundation::Interop;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Collections::Specialized;

generic<typename TItem> ReadOnlyCollection<TItem>::ReadOnlyCollection()
{
}

// virtual
generic<typename TItem> void ReadOnlyCollection<TItem>::Add(TItem item)
{
}

// virtual
generic<typename TItem> void ReadOnlyCollection<TItem>::Remove(TItem item)
{
}

// virtual
generic<typename TItem> void ReadOnlyCollection<TItem>::RemoveAt(int idx)
{
}

// virtual
generic<typename TItem> void ReadOnlyCollection<TItem>::Clear()
{
}

generic<typename TItem> System::Collections::Generic::IEnumerator<TItem>^ ReadOnlyCollection<TItem>::GetEnumerator()
{
}

generic<typename TItem> System::Collections::IEnumerator^ ReadOnlyCollection<TItem>::GetNonGenericEnumerator()
{
}

generic<typename TItem> TItem ReadOnlyCollection<TItem>::default::get(int index)
{
}

generic<typename TItem> int ReadOnlyCollection<TItem>::Count::get() 
{
}

generic<typename TItem> void ReadOnlyCollection<TItem>::CollectionChanged::add(System::Collections::Specialized::NotifyCollectionChangedEventHandler^ newHandler)
{
}

generic<typename TItem> void ReadOnlyCollection<TItem>::CollectionChanged::remove(System::Collections::Specialized::NotifyCollectionChangedEventHandler^ oldHandler)
{
}

generic<typename TItem> void ReadOnlyCollection<TItem>::CollectionChanged::raise(System::Object^ sender, NotifyCollectionChangedEventArgs^ e)
{
}

generic<typename TItem> void ReadOnlyCollection<TItem>::OnCollectionChanged(NotifyCollectionChangedAction action, TItem item, int index)
{

}
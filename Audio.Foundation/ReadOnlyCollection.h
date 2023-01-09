#pragma once

using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Collections::Specialized;

namespace Audio
{
	namespace Foundation
	{
		namespace Templates
		{
			generic<typename TItem> public ref class ReadOnlyCollection :
				public System::Collections::Generic::IEnumerable<TItem>, public System::Collections::IEnumerable,
				public System::Collections::Specialized::INotifyCollectionChanged
			{
			public:
				virtual System::Collections::Generic::IEnumerator<TItem>^ GetEnumerator() = System::Collections::Generic::IEnumerable<TItem>::GetEnumerator
				{
					return m_itemList->GetEnumerator();
				}

				virtual System::Collections::IEnumerator^ GetNonGenericEnumerator() = System::Collections::IEnumerable::GetEnumerator
				{
					return m_itemList->GetEnumerator();
				}

				virtual event System::Collections::Specialized::NotifyCollectionChangedEventHandler^ CollectionChanged
				{
					void add(System::Collections::Specialized::NotifyCollectionChangedEventHandler^ newHandler)
					{
						m_collectionChanged = static_cast<System::Collections::Specialized::NotifyCollectionChangedEventHandler^> (System::Delegate::Combine(m_collectionChanged, newHandler));
					}
					void remove(System::Collections::Specialized::NotifyCollectionChangedEventHandler^ oldHandler)
					{
						m_collectionChanged = static_cast<System::Collections::Specialized::NotifyCollectionChangedEventHandler^> (System::Delegate::Remove(m_collectionChanged, oldHandler));
					}
					void raise(System::Object^ sender, System::Collections::Specialized::NotifyCollectionChangedEventArgs^ e)
					{
						System::Collections::Specialized::NotifyCollectionChangedEventHandler^ handler = m_collectionChanged;

						if (handler != nullptr)
						{
							handler->Invoke(sender, e);
						}
					}
				}

				property TItem default[int]
				{
					TItem get(int index)
					{
						return m_itemList[index];
					}
				}

				property int Count
				{
					int get()
					{
						return m_itemList->Count;
					}
				}

				ReadOnlyCollection()
				{
					m_itemList = gcnew System::Collections::Generic::List<TItem>();
				}

				virtual void Add(TItem item)
				{
					m_itemList->Add(item);

					OnCollectionChanged(NotifyCollectionChangedAction::Add, item, m_itemList->Count - 1);
				}

				virtual void RemoveAt(int idx)
				{
					TItem item = m_itemList[idx];
					m_itemList->RemoveAt(idx);

					OnCollectionChanged(NotifyCollectionChangedAction::Remove, item, idx);
				}

				virtual void Remove(TItem item)
				{
					int index = m_itemList->IndexOf(item);

					if (index >= 0)
						RemoveAt(index);

				}

				virtual void Clear()
				{
					OnCollectionChanged(NotifyCollectionChangedAction::Reset, TItem(), -1);
					m_itemList->Clear();
				}

			protected:
				virtual void OnCollectionChanged(System::Collections::Specialized::NotifyCollectionChangedAction action, TItem item, int index)
				{
					NotifyCollectionChangedEventHandler^ handler = m_collectionChanged;

					if (nullptr != handler)
					{
						NotifyCollectionChangedEventArgs^ e = gcnew NotifyCollectionChangedEventArgs(action, item, index);

						handler(this, e);
					}
				}

			private:
				System::Collections::Generic::List<TItem>^ m_itemList;
				System::Collections::Specialized::NotifyCollectionChangedEventHandler^ m_collectionChanged;
			};
		}
	}
}

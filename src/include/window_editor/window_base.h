/* Šî’êƒNƒ‰ƒX‚ğì¬‚·‚é */

#ifndef WINDOW_BASE_H
#define WINDOW_BASE_H

namespace n_windowbase
{
	class window_base
	{
	public:
		virtual ~window_base() = default;
		virtual void Render() = 0;
	};
}
#endif // !WINDOW_BASE_H
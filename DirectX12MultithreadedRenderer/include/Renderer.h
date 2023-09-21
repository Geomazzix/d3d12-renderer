#pragma once

namespace mtd3d
{
	/// <summary>
	/// A multithreaded renderer instance that takes care of rendering the primitives in the scene.
	/// </summary>
	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		void Render();
	};
}
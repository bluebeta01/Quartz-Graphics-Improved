#pragma once
namespace Quartz::Editor
{
class EditorWindow
{
public:
	virtual void update() = 0;
	virtual void render() = 0;

	bool getVisible() const { return m_visible; }
	void setVisible(bool visible) { m_visible = visible; }

protected:
	bool m_visible = false;
};
}
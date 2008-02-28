#coding: utf-8
import wx
from wx import xrc

SDLContext = wx.Panel


class Kokompe(wx.App):
	def OnInit(self):
		self.res = xrc.XmlResource("KokompeUI.xrc")
		self.InitFrame()
		self.InitMenu()
		return True
		
	def InitFrame(self):
		self.frame = self.res.LoadFrame(None, "Kokompe")
		self.frame.Show()
		pass

	def InitMenu(self):
		# Init main menu here..
		self.frame.Bind(wx.EVT_MENU, self.GITOpenDialog, id=xrc.XRCID("KOMENU_PROJECT_OPEN"))
		self.frame.Bind(wx.EVT_MENU, self.OnAboutBox, id=xrc.XRCID("KOMENU_HELP_ABOUT"))
		pass
		
	def GITOpenDialog(self, evt):
		pass
		
	def OnAboutBox(self, event):
		description = """"""
		licence = """Kokompe is free software; you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation; 
either version 2 of the License, or (at your option) any later version.

Kokompe is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
See the GNU General Public License for more details. You should have received a copy of 
the GNU General Public License along with File Hunter; if not, write to 
the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA"""

		info = wx.AboutDialogInfo()

		info.SetIcon(wx.Icon('icons/kokompe.png', wx.BITMAP_TYPE_PNG))
		info.SetName('Kokompe')
		info.SetVersion('0.1')
		info.SetDescription(description)
		info.SetCopyright('© 2008 ')
		info.SetWebSite('http://www.kokompe.net')
		info.SetLicence(licence)
		info.AddDeveloper('David Dalrymple')
		info.AddDeveloper('Ara Knaian')
		info.AddDeveloper('Smári McCarthy')
		info.AddDeveloper('...')
		# info.AddDocWriter('Smári McCarthy')
		# info.AddArtist(' ')
		# info.AddTranslator(' ')

		wx.AboutBox(info)

		
def main():
	app = Kokompe(0)
	app.MainLoop()

if __name__ == '__main__':
	main()

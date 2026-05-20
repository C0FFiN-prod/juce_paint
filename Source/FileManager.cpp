#include "../Include/FileManager.h"

#define _(x) juce::String::fromUTF8(u8#x)

FileManager::FileManager(juce::Component& windowToRename,
	juce::String programName,
	LoadImageCallback onLoad,
	GetImageCallback onGetImage)
	: window(windowToRename),
	programName(programName),
	loadCallback(std::move(onLoad)),
	getImageCallback(std::move(onGetImage))
{
	jassert(juce::MessageManager::getInstance()->isThisTheMessageThread());
	updateWindowTitle();
}

void FileManager::updateWindowTitle()
{
	juce::String fileName = (!currentFile.exists()) ? _(Без названия) : currentFile.getFileName();
	juce::String title = juce::String(isModified ? "*" : "") + fileName + " - " + programName;

	window.setName(title);
}

void FileManager::showSavePrompt(std::function<void(bool)> onDecision)
{
	jassert(juce::MessageManager::getInstance()->isThisTheMessageThread());

	juce::String fileName = (!currentFile.exists()) ? _("Без названия") : currentFile.getFileName();
	juce::String msg = _(Хотите сохранить работу?\nЕсть несохраненные изменения в ) + fileName + ".";
	juce::AlertWindow::showYesNoCancelBox(
		juce::AlertWindow::QuestionIcon, _(Сохранение), msg,
		_(Сохранить),
		_(Не сохранять),
		_(Отмена), 
		nullptr,
		juce::ModalCallbackFunction::create(
		[this, onDecision](int result)
		{
			if (result == 1) // Сохранить
			{
				if (!currentFile.exists()) 
					saveAsFile([onDecision]() { onDecision(true); });
				else if (performSave(currentFile)) 
					onDecision(true);
			}
			else if (result == 2) // Не сохранять
			{
				onDecision(true);
			}
			else // Отмена
			{
				onDecision(false);
			}
		}));
}

bool FileManager::performSave(const juce::File& file)
{
	if (!getImageCallback) return false;
	juce::Image img = getImageCallback();
	if (!img.isValid()) return false;

	juce::FileOutputStream out(file);
	if (!out.openedOk()) return false;

	out.setPosition(0);
	out.truncate();
	juce::PNGImageFormat format;
	bool result = format.writeImageToStream(img, out);
	if (result) out.flush();
	return result;
}

void FileManager::newFile()
{
	if (isModified)
	{
		showSavePrompt([this](bool shouldProceed) {
			if (shouldProceed)
			{
				currentFile = juce::File();
				markAsChanged(false);
				loadCallback(juce::Image(juce::Image::ARGB, 200, 200, true));
			}
			});
	}
	else
	{
		currentFile = juce::File();
		markAsChanged(false);
		loadCallback(juce::Image(juce::Image::ARGB, 200, 200, true));
	}
}

void FileManager::openFile()
{
	if (isModified)
	{
		showSavePrompt([this](bool shouldProceed) {
			if (shouldProceed) promptForOpen();
			});
	}
	else
	{
		promptForOpen();
	}
}

void FileManager::promptForOpen()
{
	chooser.reset(new juce::FileChooser(_(Открыть файл),
		juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),
		"*.png;*.jpg;*.jpeg;*.bmp;*.gif", true));
	auto folderChooserFlags = FileBrowserComponent::canSelectFiles | FileBrowserComponent::openMode;

	chooser->launchAsync(folderChooserFlags, [this](const FileChooser& c)
		{
			juce::File file(c.getResult());
			juce::FileInputStream stream(file);
			if (stream.openedOk())
			{
				auto img = juce::ImageFileFormat::loadFrom(stream);
				if (img.isValid())
				{
					currentFile = file;
					loadCallback(img);
					markAsChanged(false);
				}
				else
				{
					juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, _(Ошибка), _(Не удалось открыть изображение.));
				}
			}
		});
}

void FileManager::saveFile(std::function<void()> onSave)
{
	if (!currentFile.exists() || !currentFile.existsAsFile())
		saveAsFile(onSave);
	else if (performSave(currentFile) && onSave) onSave();
}

void FileManager::saveAsFile(std::function<void()> onSave)
{
	chooser.reset(new juce::FileChooser(_(Сохранить как...),
		!currentFile.exists() ?
		juce::File::getSpecialLocation(juce::File::userDocumentsDirectory) : currentFile.getParentDirectory(),
		"*.png", true));
	auto folderChooserFlags = FileBrowserComponent::canSelectFiles | FileBrowserComponent::saveMode | FileBrowserComponent::warnAboutOverwriting;
	
	chooser->launchAsync(folderChooserFlags, [this, onSave](const FileChooser& c)
		{
			juce::File file(c.getResult());
			if (!file.hasFileExtension(".png")) file = file.withFileExtension(".png");

			if (performSave(file))
			{
				currentFile = file;
				markAsChanged(false);
				if (onSave) onSave();
			}
			else
			{
				juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, _(Ошибка), _(Не удалось сохранить файл.));
			}
		});
}

void FileManager::markAsChanged(bool changed)
{
	jassert(juce::MessageManager::getInstance()->isThisTheMessageThread());
	isModified = changed;
	updateWindowTitle();
}

bool FileManager::handleWindowClose()
{
	if (!isModified) return true;

	showSavePrompt([this](bool shouldProceed) {
		if (shouldProceed)
			JUCEApplication::getInstance()->systemRequestedQuit();
		});
	return false;
}
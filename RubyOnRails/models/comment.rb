class Comment < ApplicationRecord
	belongs_to :proyect
	belongs_to :user, optional: true # Se tuvo que dejar con optional :true porque si no no permitía crearlos. Se verifica en BE y FE.

	validates :comment_text, presence: true, allow_blank: false

	scope :submitted_to, (lambda { |proyect|
		where(proyect: proyect) if proyect
	})

end
